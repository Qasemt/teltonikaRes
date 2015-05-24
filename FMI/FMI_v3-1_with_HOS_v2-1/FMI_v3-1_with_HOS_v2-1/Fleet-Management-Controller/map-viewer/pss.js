jQuery(function() {
    jQuery('#pss-menu .sendAll').on('click',sendAllRoutes);
    jQuery('#pss-menu .deleteAll').on('click',deleteAllRoutes);

    if (jQuery('#pss tr').length > 0) {
        setTimeout(function() {
            jQuery('#pss-menu .all-action').removeClass('disabled');
        }, 500);
    }

    jQuery('.addressField').keydown(function(event) {
        if (event.keyCode == 13) {
            event.preventDefault();
            jQuery(jQuery(this).attr('data-trigger')).trigger('click');
        }
    });
    jQuery('.addressField').keyup(function(event) {
        if (jQuery.trim(jQuery(this).val()).length > 0) {
            jQuery(jQuery(this).attr('data-trigger')).removeAttr('disabled');
        }
        else {
            jQuery(jQuery(this).attr('data-trigger')).attr('disabled','disabled');
        }
    });

    jQuery('#routeInfoDlg').on('shown', function() {
        jQuery('#routeInfoDlg .input').val('');
        jQuery('#addRouteID').focus();
    });

    jQuery('#choosePSSCoordinates').on('shown', function() {
        jQuery('#locateAddress').attr('disabled','disabled');
        jQuery('#choosingFinalDestination').trigger('click');
        jQuery('#addressToLocate').val('');
        jQuery('#addressToLocate').focus();
        jQuery('#choosingIntermediateDestinations,#choosingShapingPoints').attr('disabled','disabled');
    });

    jQuery('#clearDrivingDirections').on('click', function() {
        jQuery('#drivingDirectionsContainer').hide();
        var dm = jQuery('#map_canvas').gmap('get', 'services > DirectionsManager', new Microsoft.Maps.Directions.DirectionsManager(getMapObject()));
        if (dm != null) {
            dm.resetDirections();
        }
    });

    jQuery('#choosePSSCoordinates .btn-group[data-toggle="buttons-radio"] button').on('click',function(e) {
        switch (this.id) {
            case 'choosingFinalDestination':
                jQuery('#destinationName').val(routes[-1].name).focus().select();
                jQuery('#choosePSSCoordinates').addClass('choosingDestination');
                break;
            case 'choosingIntermediateDestinations':
                var greatest = 0;
                var route = routes[-1];
                for (var i=0, cnt=route.intermediates.length; i<cnt; ++i) {
                    if (route.intermediates[i].type === 'intermediate') {
                        greatest = Math.max(greatest,route.intermediates[i].id);
                    }
                }
                jQuery('#destinationName').val('number '+(greatest+1)).focus().select();
                jQuery('#choosePSSCoordinates').addClass('choosingDestination');
                break;
            case 'choosingShapingPoints':
                jQuery('#choosePSSCoordinates').removeClass('choosingDestination');
                break;
        }
    });

    jQuery('#chooseRouteEstimateOriginFromAddress').on('click', locateAddressForEstimateOrigin);
    jQuery('#chooseRouteEstimateOrigin .btn-primary').on('click', estimateFromFirstDest);
});
// the next few functions take input from the page and communicate it to the hosting application
/*
 * send route information to the attached device
 */
function sendRouteToDevice(id,callback) {
    location.href = 'app:sendRouteToDevice('+id+(callback?','+callback:'')+')';
}
/*
 * callback for sendRouteToDevice
 */
function sentRouteToDevice(result) {

}
/*
 * delete the specified region from the attached device
 */
function deleteRouteFromDevice() {
    var routeKey = jQuery('#morePSSActionsKey').val();
    jQuery('#morePSSActions').modal('hide');
    location.href = 'app:deleteRouteFromDevice('+routes[routeKey].id+')';
}
/*
 * callback for deleteRouteFromDevice
 */
function deletedRouteFromDevice(result) {

}
// the next few functions iterate on all of the appropriate items in the catalog, calling the one-off methods listed above
function sendAllRoutes(e) {
    if (jQuery(e.currentTarget).parent().hasClass('disabled')) {
        return;
    }
    for (var key in routes) {
        sendRouteToDevice(routes[key].id);
    }
}
function deleteAllRoutes(e) {
    if (jQuery(e.currentTarget).parent().hasClass('disabled')) {
        return;
    }
    for (var key in routes) {
        deleteRouteFromDevice(routes[key].id);
    }
}
/*
 * add or update the route by id
 */
function addRouteToCatalog(id,name,description,end,intermediates) {
    location.href = 'app:beginAddRouteToCatalog('+id+',"'+name+'","'+(description == '' ? '' : description)+'",'+
        parseInt(degreesToSemicircles(end.latitude),10)+','+
        parseInt(degreesToSemicircles(end.longitude),10)+')';
    for (var i=0, cnt=intermediates.length; i<cnt; ++i) {
        location.href = 'app:continueAddRouteToCatalog('+((intermediates[i].type === 'shaping') ? 1 : 0)
            + ',"'+intermediates[i].name+'"'
            + ','+degreesToSemicircles(intermediates[i].position.latitude)
            + ','+degreesToSemicircles(intermediates[i].position.longitude)
            + ')';
    }
    location.href = 'app:endAddRouteToCatalog()';
    jQuery('#pss-menu .all-action').removeClass('disabled');
}
function removeRouteFromCatalog(routeKey) {
    if (routeKey != -1) {
        location.href = 'app:removeRouteFromCatalog('+routes[routeKey].id+',routeRemovedFromCatalog)';
    }
    else {
        delete routes[routeKey];
        removeRouteFromView(routeKey);
    }
}
function routeRemovedFromCatalog(removed,routeKey) {
    if (removed) {
        delete routes[routeKey];
        removeRouteFromView(routeKey);
    }
}
function removeIntermediateFromRoute(routeKey,itemId) {
    var route = routes[routeKey];
    for (var i=0, cnt=route.intermediates.length; i<cnt; ++i) {
        if (route.intermediates[i].id === itemId) {
            while (i+1<cnt) {
                route.intermediates[i] = route.intermediates[++i];
            }
            route.intermediates.length--;
            break;
        }
    }
    //save
    if (routeKey > 0) {
        addRouteToCatalog(route.id,route.name,route.descr,route.end,route.intermediates);
    }
    //remove artifacts from view
    var toRemove = [];
    eachEntity(function(idx,entity) {
        if (entity.interId === itemId) {
            toRemove[toRemove.length] = idx;
        }
    });
    var map = getMapObject();
    for (var idx = toRemove.length-1; idx >= 0; idx--) {
        map.entities.removeAt(toRemove[idx]);
    }
    redrawRoute(routeKey,route);
}
function addDestMarker(routeKey,route,withActions) {
    var map = getMapObject();
    var box = new Microsoft.Maps.Infobox(route.end,{
        title: route.name,
        description: route.descr,
        typeName: 'mini'
    });
    if (withActions) {
        box.setOptions({
            actions: [
                { label: 'send', eventHandler: function(evt) { sendRouteToDevice(route.id); } },
                { label: 'remove', eventHandler: function(evt) { removeRouteFromCatalog(routeKey); } },
                { label: 'snap to road', eventHandler: function(evt) { snapMarkerToRoad(routeKey,-1); } },
                { label: 'estimate path', eventHandler: function(evt) { estimateRoute(routeKey); } },
                { label: 'more...', eventHandler: function(evt) { showMoreActions(routeKey); } }
            ]
        });
    }
    box.routeKey = routeKey;
    box.interId = -1;
    map.entities.push(box);
    var marker = new Microsoft.Maps.Pushpin(route.end, {
        draggable: true,
        icon: 'blank.png',
        typeName: 'marker-icon destination' + (routeKey === -1 ? ' placing' : ''),
        width: 22, height: 22,
        anchor: new Microsoft.Maps.Point(11,15),
        infobox: box
    });
    marker.routeKey = routeKey;
    marker.interId = -1;
    map.entities.push(marker);
    Microsoft.Maps.Events.addThrottledHandler(marker, 'drag', function(e) { interceptCoordsChosen(); finalDestLocChanged(e.entity); }, 250);
    jQuery('#choosingIntermediateDestinations,#choosingShapingPoints').removeAttr('disabled');
}
function addIntermediateMarker(map,routeKey,route,inter) {
    var box = new Microsoft.Maps.Infobox(inter.position,{
        title: inter.name,
        description: inter.type,
        typeName: 'mini'
    });
    box.setOptions({
        actions: [
            { label: 'remove', eventHandler: function(evt) { removeIntermediateFromRoute(routeKey,inter.id); } },
            { label: 'snap to road', eventHandler: function(evt) { snapMarkerToRoad(routeKey,inter.id); } }
        ]
    });
    box.routeKey = routeKey;
    box.interId = inter.id;
    map.entities.push(box);
    marker = new Microsoft.Maps.Pushpin(inter.position, {
        draggable: true,
        icon: 'blank.png',
        typeName: 'marker-icon ' + inter.type + (routeKey === -1 ? ' placing' : ''),
        width: 22, height: 22,
        anchor: new Microsoft.Maps.Point(12,15)
        ,infobox: box
    });
    marker.routeKey = routeKey;
    marker.interId = inter.id;
    map.entities.push(marker);
    Microsoft.Maps.Events.addThrottledHandler(marker, 'drag', function(e) { interceptCoordsChosen(); interDestLocChanged(e.entity); }, 250);
}
function routeCoordsChosen(map,loc) {
    var route = routes[-1];
    if (jQuery('#choosingFinalDestination').hasClass('active')) {
        route.name = jQuery('#destinationName').val();
        // if destination is not already set
        if (route.end) {
            route.end = loc;
            //move destination marker
            eachEntity(function(idx,item) {
                if (item instanceof Microsoft.Maps.Pushpin && item.routeKey === -1 && item.interId === -1) {
                    item.setLocation(loc);
                    item._infobox.setLocation(loc);
                }
            });
            redrawRoute(-1,route);
        }
        else {
            route.end = loc;
            addDestMarker(-1,route,false);

            jQuery('#choosingIntermediateDestinations').trigger('click');
        }
    }
    else {
        var type = jQuery('#choosingIntermediateDestinations').hasClass('active') ? 'intermediate' : 'shaping';
        var greatest = 0;
        var greatestest = 0;
        for (var i=0, cnt=route.intermediates.length; i<cnt; ++i) {
            if (route.intermediates[i].type === 'intermediate') {
                greatest = Math.max(greatest,route.intermediates[i].id);
            }
            greatestest = Math.max(greatestest,route.intermediates[i].id);
        }
        var name = jQuery.trim(jQuery('#destinationName').val());
        if (name === '') {
            name = 'number '+(greatest+1);
        }
        route.intermediates[route.intermediates.length] = {
            "id": greatestest+1,
            "type": type,
            "name": name,
            "position": loc
        };
        addIntermediateMarker(map,-1,route,route.intermediates[route.intermediates.length-1]);
        redrawRoute(-1,route);
        jQuery('#destinationName').val('number '+(greatest+2)).focus().select();
    }
}
var routes = {};
function redrawRoute(routeKey,route) {
    var toRemove = [];
    eachEntity(function(idx,entity) {
        if (entity instanceof Microsoft.Maps.Polyline && entity.routeKey === routeKey) {
            toRemove[toRemove.length] = idx;
        }
    });
    var map = getMapObject();
    for (var idx = toRemove.length-1; idx >= 0; idx--) {
        map.entities.removeAt(toRemove[idx]);
    }
    if (route.end && route.intermediates.length > 0) {
        drawRoute(routeKey,route);
    }
}
function drawRoute(routeKey,route) {
    var map = getMapObject();
    var vertices = [route.end];
    for (var i=0, cnt=route.intermediates.length; i<cnt; ++i) {
        vertices[vertices.length] = route.intermediates[i].position;
    }
    var polyline = new Microsoft.Maps.Polyline(vertices, {
        strokeColor: new Microsoft.Maps.Color(200,0,100,100),
        strokeThickness: 2
    });
    polyline.routeKey = routeKey;
    map.entities.push(polyline);
}
function showRoute(routeKey,route) {
    //store in memory
    routes[routeKey] = route;
    var map = getMapObject();
    addDestMarker(routeKey,route,true);
    drawRoute(routeKey,route);

    for (var i=0, cnt=route.intermediates.length; i<cnt; ++i) {
        addIntermediateMarker(map,routeKey,route,route.intermediates[i]);
    }
}
function removeRouteFromView(routeKey) {
    var toRemove = [];
    eachEntity(function(idx,item) {
        if (item.routeKey == routeKey) {
            toRemove[toRemove.length] = idx;
        }
    });
    var map = getMapObject();
    for (var idx = toRemove.length-1; idx >= 0; idx--) {
        map.entities.removeAt(toRemove[idx]);
    }
}
function finalDestLocChanged(marker) {
    var routeKey = marker._infobox.routeKey;
    var route = routes[routeKey];
    route.end = marker.getLocation();
    marker._infobox.setLocation(marker.getLocation());
    //save changes if not in the process of creating a new route
    if (routeKey > 0) {
        addRouteToCatalog(route.id,route.name,route.descr,route.end,route.intermediates);
    }
    //re-show the route
    redrawRoute(routeKey,route);
}
function interDestLocChanged(marker) {
    var routeKey = marker.routeKey;
    var route = routes[routeKey];
    for (var i=0,cnt=route.intermediates.length; i<cnt; ++i) {
        if (route.intermediates[i].id === marker.interId) {
            route.intermediates[i].position = marker.getLocation();
            break;
        }
    }
    marker._infobox.setLocation(marker.getLocation());
    //save changes if not in the process of creating a new route
    if (routeKey > 0) {
        addRouteToCatalog(route.id,route.name,route.descr,route.end,route.intermediates);
    }
    //re-show the route
    redrawRoute(routeKey,route);
}
function beginAddRouteStep1() {
    jQuery('#routeInfoDlg').modal();
}
function cancelAddRouteStep1() {
    jQuery('#routeInfoDlg').modal('hide');
}
function addRouteStep2b() {
    if (jQuery.trim(jQuery('#addRouteID').val()).length == 0 ||
        jQuery.trim(jQuery('#addRouteID').val()) != parseInt(jQuery.trim(jQuery('#addRouteID').val()),10)) {
        jQuery('#addRouteID').focus().select();
        return;
    }
    if (jQuery.trim(jQuery('#addRouteName').val()).length == 0) {
        jQuery('#addRouteName').focus().select();
        return;
    }
    //warn if new route ID is not unique
    var newRouteID = parseInt(jQuery.trim(jQuery('#addRouteID').val()),10);
    for (var key in routes) {
        if (routes[key].id == newRouteID) {
            alert('The ID ' + newRouteID + ' is already used by another entry.');
            jQuery('#addRouteID').focus().select();
            return;
        }
    }
    jQuery('#routeInfoDlg').modal('hide');
    routes[-1] = {
        "id": jQuery.trim(jQuery('#addRouteID').val()),
        "name": jQuery.trim(jQuery('#addRouteName').val()),
        "descr": jQuery.trim(jQuery('#addRouteDescription').val()),
        "intermediates": []
    };
    beginChoosingCoords('choosePSSCoordinates',routeCoordsChosen);
    jQuery('#choosePSSCoordinates').trigger('shown');
}
function doLocateAddress() {
    var address = jQuery('#addressToLocate').val();
    if (jQuery.trim(address).length == 0) {
        return;
    }
    disableSubmits('#choosePSSCoordinates');
    jQuery('#locateAddress').button('loading');
    //locate coordinates from address
    jQuery('#map_canvas').gmap('search', { 'address': address },
        function(result, status) {
            if (status === 'OK') {
                //give preference to the Interpolation coordinates (which should lie on the street)
                var coords = result[0].resources[0].point.coordinates;
                if (result[0].resources[0].geocodePoints.length) {
                    coords = null;
                    for (var i=0, cnt=result[0].resources[0].geocodePoints.length; i<cnt; i++) {
                        if (coords === null || result[0].resources[0].geocodePoints[i].calculationMethod == 'Interpolation') {
                            coords = result[0].resources[0].geocodePoints[i].coordinates;
                        }
                    }
                }
                var location = new Microsoft.Maps.Location(coords[0],coords[1]);
                setCenterAndZoom(location.latitude, location.longitude, 15);
                routeCoordsChosen(getMapObject(), location);
                jQuery('#addressToLocate').val('');
            }
            else {
                jQuery('#findAddressErrorMessage').fadeIn('fast');
            }
            enableSubmits('#choosePSSCoordinates');
            jQuery('#locateAddress').button('reset');
            if (status === 'OK') {
                setTimeout(function() {
                    jQuery('#locateAddress').attr('disabled','disabled');
                }, 250);
            }
        }
    );
}
function snapMarkerToRoad(routeKey,itemId) {
    var marker = null;
    eachEntity(function(idx,item) {
        if (item instanceof Microsoft.Maps.Pushpin && item.routeKey === routeKey && item.interId === itemId) {
            marker = item;
        }
    });
    if (marker == null) return;
    var coords = marker.getLocation().latitude+','+marker.getLocation().longitude;
    jQuery('#map_canvas').gmap('loadDirections', { 'origin': coords, 'destination': coords },
        function(result, status) {
            if (status === 'OK') {
                var newCoords = result[0].resources[0].routePath.line.coordinates[0];
                marker.setLocation(new Microsoft.Maps.Location(newCoords[0],newCoords[1]));
                if (itemId === -1) {
                    finalDestLocChanged(marker);
                }
                else {
                    interDestLocChanged(marker);
                }
            }
        }
    );
}
function estimateRoute(routeKey) {
    jQuery('#estimateRouteToKey').val(routeKey);
    beginChoosingCoords('chooseRouteEstimateOrigin',function(map,loc) {
        estimateRouteFrom(jQuery('#estimateRouteToKey').val(),loc,function(status) {
            if (status === 'OK') {
                cancelEstimateRoute();
            }
        });
    });
}
function locateAddressForEstimateOrigin() {
    var address = jQuery('#originAddressToLocate').val();
    if (jQuery.trim(address).length == 0) {
        return;
    }
    disableSubmits('#chooseRouteEstimateOrigin');
    jQuery('#chooseRouteEstimateOriginFromAddress').button('loading');
    estimateRouteFrom(jQuery('#estimateRouteToKey').val(),address,function(status) {
        enableSubmits('#chooseRouteEstimateOrigin');
        jQuery('#chooseRouteEstimateOriginFromAddress').button('reset');
        if (status === 'OK') {
            cancelEstimateRoute();
        }
    });
}
function cancelEstimateRoute() {
    jQuery('#estimateRouteToKey').val('');
    jQuery('#chooseRouteEstimateOrigin').hide();
    cancelChoosingCoords();
}
function estimateFromFirstDest() {
    estimateRouteFrom(jQuery('#estimateRouteToKey').val(),null,function(status) {
        enableSubmits('#chooseRouteEstimateOrigin');
        jQuery('#chooseRouteEstimateOrigin .btn-primary').button('reset');
        if (status === 'OK') {
            cancelEstimateRoute();
        }
    });
}
function estimateRouteFrom(routeKey,origin,callback) {
    var route = routes[routeKey];
    var directionCallback = function() {
        var dm = jQuery('#map_canvas').gmap('get', 'services > DirectionsManager', new Microsoft.Maps.Directions.DirectionsManager(getMapObject()));
        dm.resetDirections();
        dm.setRequestOptions(Microsoft.Maps.Directions.RouteMode.driving);
        if (origin !== null) {
            dm.addWaypoint(new Microsoft.Maps.Directions.Waypoint(( typeof origin === 'string' ) ? { 'address': origin } : { 'location': origin }));
        }
        for (var i=route.intermediates.length-1; i>=0; --i) {
            dm.addWaypoint(new Microsoft.Maps.Directions.Waypoint({'location': route.intermediates[i].position, 'isViapoint': route.intermediates[i].type === 'shaping'}));
        }
        dm.addWaypoint(new Microsoft.Maps.Directions.Waypoint({'location': route.end }));

        dm.setRenderOptions({'itineraryContainer': document.getElementById('drivingDirectionsSteps')});
        if (callback) {
            //TODO figure out how to handle invalid directions. There doesn't seem to be an event for that
            var errorHandler = Microsoft.Maps.Events.addHandler(dm, 'directionsError', function(e) {
                callback('ERROR',e.responseCode);
                Microsoft.Maps.Events.removeHandler(errorHandler);
            });
            var successHandler = Microsoft.Maps.Events.addHandler(dm, 'directionsUpdated', function(e) {
                callback('OK');
                Microsoft.Maps.Events.removeHandler(successHandler);
            });
        }
        dm.calculateDirections();
        jQuery('#drivingDirectionsContainer').show();
    }
    if ( !jQuery('#map_canvas').gmap('get', 'services > DirectionsManager') ) {
        Microsoft.Maps.loadModule('Microsoft.Maps.Directions', { callback: directionCallback });
    } else {
        directionCallback();
    }
}
function showMoreActions(routeKey) {
    jQuery('#morePSSActionsKey').val(routeKey);
    jQuery('#morePSSActions').modal();
}
function activateStop(callback) {
    var routeKey = jQuery('#morePSSActionsKey').val();
    jQuery('#morePSSActions').modal('hide');
    location.href = 'app:activateStopOnDevice('+routes[routeKey].id+(callback?','+callback:'')+')';
}
function completeStop(callback) {
    var routeKey = jQuery('#morePSSActionsKey').val();
    jQuery('#morePSSActions').modal('hide');
    location.href = 'app:completeStopOnDevice('+routes[routeKey].id+(callback?','+callback:'')+')';
}
function repositionStop(callback) {
    var routeKey = jQuery('#morePSSActionsKey').val();
    var newPosition = jQuery.trim(jQuery('#newPSSPosition').val());
    if (newPosition === null || newPosition === '' || parseInt(newPosition,10) != newPosition) {
        jQuery('#newPSSPosition').focus().select();
        return;
    }
    jQuery('#morePSSActions').modal('hide');
    location.href = 'app:moveStopOnDevice('+routes[routeKey].id+','+newPosition+(callback?','+callback:'')+')';
}
function commitAddRoutePoints() {
    var route = routes[-1];
    if (route.intermediates.length === 0) {
        alert('You must add at least one intermediate destination');
        return;
    }
    if (route.intermediates[route.intermediates.length-1].type === 'shaping') {
        alert('The first/starting point of this route must be an intermediate destination');
        jQuery('#choosingIntermediateDestinations').trigger('click');
        return;
    }
    var shapingCount = 0;
    var intermediateCount = 0;
    for (var i=0, cnt=route.intermediates.length; i<cnt; ++i) {
        switch (route.intermediates[i].type) {
            case 'intermediate':
                shapingCount = 0;
                if (intermediateCount++ > 23) {
                    alert('No more than 24 intermediate destinations are allowed');
                    return;
                }
                break;
            case 'shaping':
                if (shapingCount++ > 100) {
                    alert('No more than 100 shaping points are allowed between intermediate destinations');
                    return;
                }
                break;
        }
    }
    addRouteToCatalog(route.id,route.name,route.descr,route.end,route.intermediates);
    cancelAddRoutePoints();
    showRoute(route.id,route);
}
function cancelAddRoutePoints() {
    //remove any temporary markers
    var toRemove = [];
    eachEntity(function(idx,item) {
        if (item.routeKey === -1) {
            toRemove[toRemove.length] = idx;
        }
    });
    var map = getMapObject();
    for (var idx = toRemove.length-1; idx >= 0; idx--) {
        map.entities.removeAt(toRemove[idx]);
    }
    delete routes[-1];
    cancelChoosingCoords();
}