/**
 *
 */

var picker = {};
/*
 * When in coordinate choosing mode, this click event handler determines if a rectangle is complete
 * and adds that rectangle as a new avoidance region to the catalog
 */
window.interceptDrag = { timer: null, intercept: false };
function coordinatesChosen(evt) {
    if (!jQuery('body').hasClass('choosingCoords')) {
        return;
    }
    if (window.interceptDrag.intercept) {
        window.interceptDrag.intercept = false;
        return;
    }
    if (!evt.originalEvent.ctrlKey) {
        return;
    }
    var map = getMapObject();
    var loc = getMapObject().tryPixelToLocation(new Microsoft.Maps.Point(evt.getX(),evt.getY()));
    picker.coordsChosenCallback(map,loc);
}
function interceptCoordsChosen() {
    window.interceptDrag.intercept = true;
}
function beginChoosingCoords(elementId,callback) {
    picker.elementId = elementId;
    jQuery('#'+elementId).show();
    jQuery('body').addClass('choosingCoords');
    jQuery('#map_canvas').addClass('placementCursor');
    picker.coordsChosenCallback = callback;
}
function cancelChoosingCoords() {
    jQuery('#'+picker.elementId).hide();
    jQuery('body').removeClass('choosingCoords');
    jQuery('#map_canvas').removeClass('placementCursor');
    picker.coordsChosenCallback = null;
    if (picker.temporary != null) {
        for (var i=0, cnt=picker.temporary.length; i<cnt; ++i) {
            getMapObject().entities.remove(picker.temporary[i]);
        }
        picker.temporary = null;
    }
    picker.anchor = null;
}

// generic map initialization function
function loadMap(mapCreds, mapReadyCallback) {
    Microsoft.Maps.loadModule('Microsoft.Maps.Themes.BingTheme', {
        callback: function() {
            jQuery('#map_canvas').gmap({
                'credentials': mapCreds,
                'enableSearchLogo': false,
                'disableBirdseye': true,
                'mapTypeId': Microsoft.Maps.MapTypeId.road,
                'useInertia': true
                ,'theme': new Microsoft.Maps.Themes.BingTheme()
            }).bind('init', function(ev, map) {
                mapReadyCallback();
                Microsoft.Maps.Events.addHandler(map,'mousemove',function(e) {
                    var mapElem = map.getRootElement();
                    if (e.targetType === 'map') {
                        if (jQuery('#map_canvas').hasClass('placementCursor') && e.originalEvent.ctrlKey) {
                            mapElem.style.cursor = 'crosshair';
                        }
                        else {
                            mapElem.style.cursor = 'drag';
                        }
                    }
                    else {
                        mapElem.style.cursor = 'pointer';
                    }
                });
                var mapEl = jQuery('#map_canvas');
                map.setOptions({'width':mapEl.width(),'height':mapEl.height()});
                jQuery(window).resize(function(evt) {
                    // update map dimensions
                    var mapEl = jQuery('#map_canvas');
                    mapEl.gmap('get','map').setOptions({'width':mapEl.width(),'height':mapEl.height()});
                });
            });
        }
    });
}

// convenience function for looking up the map
function getMapObject() {
    return jQuery('#map_canvas').gmap('get', 'map');
}
// convenience function for moving the map to a point
function setCenterAndZoom(lat,lon,zoom) {
    getMapObject().setView({
        'zoom': zoom,
        'center': new Microsoft.Maps.Location(lat,lon)
    });
}

// convenience functions for converting between coordinate units
function semicirclesToDegrees(semicircles) {
    return semicircles * 90 / ( 1 << 30 );
}
function degreesToSemicircles(degrees) {
    return degrees * ( 1 << 30 ) / 90;
}
// convenience function for converting a rectangle specified in semicircles to a Microsoft Maps rectangle
function toLocationRect(n,e,s,w) {
    return Microsoft.Maps.LocationRect.fromLocations(
        new Microsoft.Maps.Location(semicirclesToDegrees(n),semicirclesToDegrees(e)),
        new Microsoft.Maps.Location(semicirclesToDegrees(s),semicirclesToDegrees(e)),
        new Microsoft.Maps.Location(semicirclesToDegrees(s),semicirclesToDegrees(w)),
        new Microsoft.Maps.Location(semicirclesToDegrees(n),semicirclesToDegrees(w))
    );
}
function enableSubmits(el) {
    jQuery(el).removeClass('busy');
    jQuery('.btn',el).removeClass('disabled').removeAttr('disabled');
}
function disableSubmits(el) {
    jQuery(el).addClass('busy');
    jQuery('.btn',el).addClass('disabled').attr('disabled','disabled');
}
function eachEntity(visitor) {
    var entities = getMapObject().entities;
    for (var idx = 0, len = entities.getLength(); idx < len; ++idx) {
        var entity = entities.get(idx);
        if (visitor(idx,entity) < 0) {
            break;
        }
    }
}