jQuery(function() {
    jQuery('#avoidances-menu .sendAll').on('click',sendAllAvoidances);
    jQuery('#avoidances-menu .deleteAll').on('click',deleteAllAvoidances);
    jQuery('#avoidances-menu .enableAll').on('click',enableAllAvoidances);
    jQuery('#avoidances-menu .disableAll').on('click',disableAllAvoidances);

    if (jQuery('#avoidances tr').length > 0) {
        setTimeout(function() {
            jQuery('#avoidances-menu .all-action').removeClass('disabled');
        }, 500);
    }
});
// the next few functions take input from the page and communicate it to the hosting application
/*
 * send region information to the attached device
 */
function sendAvoidanceToDevice(id,name,n,e,s,w,callback) {
    location.href = 'app:sendAvoidanceToDevice('+id+',"'+name+'",'+n+','+e+','+s+','+w+(callback?','+callback:'')+')';
}
/*
 * callback for sendToDevice
 */
function sentAvoidanceToDevice(result) {

}
/*
 * delete the specified region from the attached device
 */
function deleteAvoidanceFromDevice(id,callback) {
    location.href = 'app:deleteAvoidanceFromDevice('+id+(callback?','+callback:'')+')';
}
/*
 * callback for deleteFromDevice
 */
function deletedAvoidanceFromDevice(result) {

}
/*
 * enable the specified region on the attached device
 */
function enableAvoidanceOnDevice(id,callback) {
    location.href = 'app:enableAvoidanceOnDevice('+id+(callback?','+callback:'')+')';
}
/*
 * callback for enableOnDevice
 */
function enabledAvoidanceOnDevice(result) {

}
/*
 * disable the specified region on the attached device
 */
function disableAvoidanceOnDevice(id,callback) {
    location.href = 'app:disableAvoidanceOnDevice('+id+(callback?','+callback:'')+')';
}
/*
 * callback for disableOnDevice
 */
function disabledAvoidanceOnDevice(result) {

}
// the next few functions iterate on all of the items in the catalog, calling the one-off methods listed above
function sendAllAvoidances(e) {
    if (jQuery(e.currentTarget).parent().hasClass('disabled')) {
        return;
    }
    jQuery('#avoidances tr').each(function(idx,item) {
        sendAvoidanceToDevice(
            jQuery(item).attr('region-id'),
            jQuery('.region-name', item).text(),
            jQuery('.region-north', item).text(),
            jQuery('.region-east', item).text(),
            jQuery('.region-south', item).text(),
            jQuery('.region-west', item).text()
        );
    });
}
function deleteAllAvoidances(e) {
    if (jQuery(e.currentTarget).parent().hasClass('disabled')) {
        return;
    }
    jQuery('#avoidances tr').each(function(idx,item) {
        deleteAvoidanceFromDevice(jQuery(item).attr('region-id'));
    });
}
function disableAllAvoidances(e) {
    if (jQuery(e.currentTarget).parent().hasClass('disabled')) {
        return;
    }
    jQuery('#avoidances tr').each(function(idx,item) {
        disableAvoidanceOnDevice(jQuery(item).attr('region-id'));
    });
}
function enableAllAvoidances(e) {
    if (jQuery(e.currentTarget).parent().hasClass('disabled')) {
        return;
    }
    jQuery('#avoidances tr').each(function(idx,item) {
        enableAvoidanceOnDevice(jQuery(item).attr('region-id'));
    });
}
/**
 * direct the application to store the new region
 */
function addAvoidanceToCatalog(id,title,description,northwest,southeast) {
    location.href = 'app:addAvoidanceToCatalog('+id+',"'+title+'","'+(description == '' ? '' : description)+'",'+
        parseInt(degreesToSemicircles(northwest.latitude),10)+','+
        parseInt(degreesToSemicircles(southeast.longitude),10)+','+
        parseInt(degreesToSemicircles(southeast.latitude),10)+','+
        parseInt(degreesToSemicircles(northwest.longitude),10)+')';
    jQuery('#avoidances-menu .all-action').removeClass('disabled');
    jQuery('#avoidances').append(
        '<tr region-id="'+id+'">'+
            '<td>'+id+'</td>'+
            '<td class="region-name">'+title+'</td>'+
            '<td class="region-descr">'+description+'</td>'+
            '<td class="region-north">'+degreesToSemicircles(northwest.latitude)+'</td>'+
            '<td class="region-east">'+degreesToSemicircles(southeast.longitude)+'</td>'+
            '<td class="region-south">'+degreesToSemicircles(southeast.latitude)+'</td>'+
            '<td class="region-west">'+degreesToSemicircles(northwest.longitude)+'</td>'+
        '</tr>'
    );
}
/**
 * direct the application to remove the region
 */
function removeAvoidanceFromCatalog(id) {
    location.href = 'app:removeAvoidanceFromCatalog('+id+',removedAvoidanceFromCatalog)';
}
/**
 * callback initiated by the application whenever region is successfully removed
 */
function removedAvoidanceFromCatalog(success,id) {
    var map = getMapObject();
    var toRemove = [];
    for (var idx = 0, len = map.entities.getLength(); idx < len; ++idx) {
        var entity = map.entities.get(idx);
        if (entity.regionId == id) {
            toRemove[toRemove.length] = idx;
        }
    }
    for (var idx = toRemove.length-1; idx >= 0; idx--) {
        map.entities.removeAt(toRemove[idx]);
    }
    jQuery('#avoidances tr[region-id="'+id+'"]').remove();
    if (jQuery('#avoidances tr').length == 0) {
        jQuery('#avoidances-menu .all-action').addClass('disabled');
    }
}

function avoidanceRegionCoordsChosen(map,loc) {
    if (picker.anchor == null) {
        // if first corner, place a marker and wait for the next corner
        picker.anchor = new Microsoft.Maps.Pushpin(loc, {
            text: 'first corner',
            draggable: true,
            icon: 'anchor.png',
            width: 22, height: 22,
            anchor: new Microsoft.Maps.Point(11,22)
        });
        map.entities.push(picker.anchor);
        picker.temporary = [picker.anchor];
        jQuery('#buildAvoidanceInstructions .step').hide();
        jQuery('#buildAvoidanceInstructions .step2').show();
    }
    else {
        // if second corner, add to catalog and to map
        var firstCorner = picker.anchor.getLocation();
        var r = Microsoft.Maps.LocationRect.fromLocations(
                    new Microsoft.Maps.Location(firstCorner.latitude,loc.longitude),
                    loc,
                    new Microsoft.Maps.Location(loc.latitude,firstCorner.longitude),
                    firstCorner);
        addAvoidanceToCatalog(
            jQuery('#addRegionID').val(),
            jQuery('#addRegionName').val(),
            jQuery('#addRegionDescription').val(),
            r.getNorthwest(),
            r.getSoutheast()
        );

        showAvoidanceRegion(parseInt(jQuery('#addRegionID').val(),10), r, jQuery('#addRegionName').val(), jQuery('#addRegionDescription').val());

        map.entities.remove(picker.anchor);
        picker.anchor = null;
        picker.temporary = null;
        cancelChoosingCoords();
    }
}
/*
 * display the specified region on the map, attaching an info box with actions for manipulation
 */
function showAvoidanceRegion(id,r,name,description) {
    var map = getMapObject();
    var box = new Microsoft.Maps.Infobox(r.getNorthwest(),{
        title: name,
        description: description,
        typeName: 'mini'
    });
    box.setOptions({
        actions: [
            { label: 'send', eventHandler: function(evt) {
                sendAvoidanceToDevice(id,name,
                    degreesToSemicircles(r.getNorth()),degreesToSemicircles(r.getEast()),
                    degreesToSemicircles(r.getSouth()),degreesToSemicircles(r.getWest()),
                    'sentAvoidanceToDevice'
                );
            } },
            { label: 'delete', eventHandler: function(evt) { deleteAvoidanceFromDevice(id,'deletedAvoidanceOnDevice'); } },
            { label: 'enable', eventHandler: function(evt) { enableAvoidanceOnDevice(id,'enabledAvoidanceOnDevice'); } },
            { label: 'disable', eventHandler: function(evt) { disableAvoidanceOnDevice(id,'disabledAvoidanceOnDevice'); } },
            { label: 'remove', eventHandler: function(evt) { removeAvoidanceFromCatalog(id); } }
        ]
    });
    var polygon = new Microsoft.Maps.Polygon([r.getNorthwest(),new Microsoft.Maps.Location(r.getNorth(),r.getEast()),r.getSoutheast(),new Microsoft.Maps.Location(r.getSouth(),r.getWest()),r.getNorthwest()], {
        fillColor: new Microsoft.Maps.Color(100,100,0,100),
        strokeColor: new Microsoft.Maps.Color(200,0,100,100),
        strokeThickness: 3,
        infobox: box
    });
    polygon.regionId = id;
    box.regionId = id;
    map.entities.push(box);
    map.entities.push(polygon);
}
// the next few functions handle the actions taken during the process of creating a new region
function beginAddRegionStep1() {
    jQuery('#regionInfoDlg input').val('');
    jQuery('#regionInfoDlg').modal();
    jQuery('#addRegionID').focus();
}
function cancelAddRegionStep1() {
    jQuery('#regionInfoDlg').modal('hide');
}
function addRegionStep2() {
    if (jQuery.trim(jQuery('#addRegionID').val()).length == 0 ||
        jQuery.trim(jQuery('#addRegionID').val()) != parseInt(jQuery.trim(jQuery('#addRegionID').val()),10)) {
        jQuery('#addRegionID').focus().select();
        return;
    }
    if (jQuery.trim(jQuery('#addRegionName').val()).length == 0) {
        jQuery('#addRegionName').focus().select();
        return;
    }
    //warn if new region ID is not unique
    var newRegionID = parseInt(jQuery.trim(jQuery('#addRegionID').val()),10);
    var unique = true;
    jQuery('#avoidances tr').each(function(idx,item) {
        if (jQuery(item).attr('region-id') == newRegionID) {
            unique = false;
            return false;
        }
    });
    if (!unique) {
        alert('The ID ' + newRegionID + ' is already used by another entry.')
        jQuery('#addRegionID').focus().select();
        return;
    }
    jQuery('#regionInfoDlg').modal('hide');
    jQuery('#buildAvoidanceInstructions .step').hide();
    jQuery('#buildAvoidanceInstructions .step1').show();
    beginChoosingCoords('buildAvoidanceInstructions',avoidanceRegionCoordsChosen);
}