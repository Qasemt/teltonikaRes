<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:msxsl="urn:schemas-microsoft-com:xslt"
    xmlns:date="urn:date"
    exclude-result-prefixes="msxsl date">
    <xsl:output method="html" encoding="utf-8" indent="yes" />

    <xsl:template match="catalog">
        <xsl:text disable-output-escaping="yes">&lt;!DOCTYPE html></xsl:text>
        <xsl:variable name="settings" select="document('../settings.xml')/settings"/>
        <html>
            <head>
                <title>Custom Avoidances</title>
                <script src="../external/jquery-1.7.2.min.js"></script>
                <!-- load the map and then add the regions from the catalog -->
                <script type="text/javascript">
                    jQuery(function() {
                        // map credentials are defined in ../settings.xml
                        loadMap('<xsl:value-of select="$settings/BING_MAPS_KEY"/>', function() {
                            Microsoft.Maps.Events.addHandler(getMapObject(), 'mouseup', coordinatesChosen);

                            var allPoints = [];
                            var r = null;
                            jQuery('#avoidances tr').each(function(idx,item) {
                                r = toLocationRect(
                                    jQuery('.region-north',item).text(),
                                    jQuery('.region-east',item).text(),
                                    jQuery('.region-south',item).text(),
                                    jQuery('.region-west',item).text()
                                );
                                showAvoidanceRegion(
                                    jQuery(item).attr('region-id'),
                                    r,
                                    jQuery('.region-name',item).text(),
                                    jQuery('.region-descr',item).text()
                                );
                                allPoints[allPoints.length] = r.getNorthwest();
                                allPoints[allPoints.length] = r.getSoutheast();
                            });
                            jQuery('#routes .route').each(function(idx,item) {
                                item = jQuery(item);
                                var end = new Microsoft.Maps.Location(semicirclesToDegrees(item.attr('end-lat')),semicirclesToDegrees(item.attr('end-lon')));
                                allPoints[allPoints.length] = end;
                                var intermediates = [];
                                jQuery('.intermediates li',item).each(function(idx2,item2) {
                                    item2 = jQuery(item2);
                                    var inter = {"id": item2.attr('inter-id'),"type": item2.attr('inter-type'),
                                        "name": jQuery('.name', item2).text(),
                                        "position": new Microsoft.Maps.Location(semicirclesToDegrees(item2.attr('lat')),semicirclesToDegrees(item2.attr('lon')))
                                    };
                                    intermediates[intermediates.length] = inter;
                                    allPoints[allPoints.length] = inter.position;
                                });
                                showRoute(item.attr('route-id'),{"id":item.attr('route-id'),"name":jQuery('.route-name',item).text(),"descr":jQuery('.route-descr',item).text(),
                                    "end": end, "intermediates": intermediates});
                            });
                            // if there are some map entities, attempt to display them all initially
                            if (allPoints.length > 0) {
                                setTimeout(function() {
                                    getMapObject().setView({
                                        'bounds': Microsoft.Maps.LocationRect.fromLocations(allPoints)
                                    });
                                }, 500);
                            }
                            // otherwise center on the home coordinates defined in ../settings.xml
                            else {
                                <xsl:if test="$settings/homeCoordinates">setCenterAndZoom(<xsl:value-of select="$settings/homeCoordinates"/>,10);</xsl:if>
                            }

                            jQuery(window).resize(function(evt) {
                                clearTimeout(window.resizeTimer);
                                window.resizeTimer = setTimeout(function() {
                                    // update map dimensions
                                    var mapEl = jQuery('#map_canvas');
                                    mapEl.gmap('get','map').setOptions({'width':mapEl.width(),'height':mapEl.height()});
                                }, 300);
                            });
                        });
                    });
                </script>
                <link rel="stylesheet" type="text/css" href="../external/bootstrap/css/bootstrap.min.css" media="screen"/>
                <link rel="stylesheet" type="text/css" href="page.css"/>
                <link rel="stylesheet" type="text/css" href="pss.css"/>
            </head>

            <body>
                <div id="toolbar">
                    <div class="btn-group" id="avoidances-menu">
                        <button class="btn" onclick="beginAddRegionStep1();" title="click to Add a Custom Avoidance" style="cursor: crosshair;">Custom Avoidances</button>
                        <button class="btn dropdown-toggle" data-toggle="dropdown" title="click to manage Custom Avoidances"><span class="caret"></span></button>
                        <ul class="dropdown-menu">
                            <li><a href="javascript:beginAddRegionStep1();">Add Custom Avoidance</a></li>
                            <li class="all-action disabled"><a href="#" class="sendAll">Send All</a></li>
                            <li class="all-action disabled"><a href="#" class="deleteAll">Delete All</a></li>
                            <li class="all-action disabled"><a href="#" class="enableAll">Enable All</a></li>
                            <li class="all-action disabled"><a href="#" class="disableAll">Disable All</a></li>
                        </ul>
                    </div>
                    <div class="btn-group" id="pss-menu">
                        <button class="btn" onclick="beginAddRouteStep1();" title="click to Add a Path-specific Stop" style="cursor: crosshair;">Path-specific Stops</button>
                        <button class="btn dropdown-toggle" data-toggle="dropdown" title="click to manage Path-specific Stops"><span class="caret"></span></button>
                        <ul class="dropdown-menu">
                            <li><a href="javascript:beginAddRouteStep1();">Add Path-specific Stop</a></li>
                            <li class="all-action disabled"><a href="#" class="sendAll">Send All</a></li>
                            <li class="all-action disabled"><a href="#" class="deleteAll">Delete All</a></li>
                        </ul>
                    </div>
                </div>
                <div id="map_canvas"></div>

                <!-- this listing is necessary so that the bulk operations (sendAllAvoidances, deleteAllAvoidances, etc.) can iterate over this data. -->
                <div style="display:none;">
                <table>
                    <thead>
                        <tr><th>ID</th><th>Name</th><th>Description</th><th>North</th><th>East</th><th>South</th><th>West</th></tr>
                    </thead>
                    <tbody id="avoidances">
                        <xsl:for-each select="region">
                        <tr region-id="{@id}">
                            <td><xsl:value-of select="@id"/></td>
                            <td class="region-name"><xsl:value-of select="@name"/></td>
                            <td class="region-descr"><xsl:value-of select="description"/></td>
                            <td class="region-north"><xsl:value-of select="@north"/></td>
                            <td class="region-east"><xsl:value-of select="@east"/></td>
                            <td class="region-south"><xsl:value-of select="@south"/></td>
                            <td class="region-west"><xsl:value-of select="@west"/></td>
                        </tr>
                        </xsl:for-each>
                    </tbody>
                </table>
                <ul id="routes"><xsl:apply-templates select="route" mode="data-elements"/></ul>
                </div>

                <!-- dialog for capturing new region information -->
                <div id="regionInfoDlg" class="modal" style="display: none;">
                    <div class="modal-header">
                        <button class="close" data-dismiss="modal">&#215;</button>
                        <h3>Add a new avoidance region</h3>
                    </div>
                    <div class="modal-body">
                        <form class="form-horizontal">
                        <fieldset>
                        <div class="control-group">
                            <label class="control-label" for="addRegionID">ID</label>
                            <div class="controls">
                                <input type="number" id="addRegionID" class="input-mini" min="0" max="65535"/>
                            </div>
                        </div>
                        <div class="control-group">
                            <label class="control-label" for="addRegionName">Name</label>
                            <div class="controls">
                                <input type="text" id="addRegionName" class="input-medium" maxLength="50"/>
                            </div>
                        </div>
                        <div class="control-group">
                            <label class="control-label" for="addRegionDescription">Description</label>
                            <div class="controls">
                                <input type="text" id="addRegionDescription" class="input-large"/>
                            </div>
                        </div>
                        </fieldset>
                        </form>
                    </div>
                    <div class="modal-footer">
                        <button class="btn btn-primary" onclick="addRegionStep2();">Next</button>
                        <button class="btn" onclick="cancelAddRegionStep1();">Cancel</button>
                    </div>
                </div>

                <div class="semi-modal" id="buildAvoidanceInstructions" style="display: none; width: 315px;">
                    <div class="modal-header">
                        <h3>Build Avoidance Region</h3>
                    </div>
                    <div class="modal-body">
                        <p class="step step1">then hold down the Ctrl key and click on the map to set the coordinates for the first corner of the region</p>
                        <p class="step step2" style="display: none;">now hold the Ctrl key and click the alternate corner of the avoidance region</p>
                    </div>
                    <div class="modal-footer">
                        <button class="btn" onclick="cancelChoosingCoords();">Cancel</button>
                    </div>
                </div>

                <div id="drivingDirectionsContainer" style="display: none;">
                    <button id="clearDrivingDirections" title="Hide Directions">&#215;</button>
                    <div id="drivingDirectionsSteps"></div>
                </div>

                <!-- dialog for capturing new route information -->
                <div id="routeInfoDlg" class="modal" style="display: none;">
                    <div class="modal-header">
                        <button class="close" data-dismiss="modal">&#215;</button>
                        <h3>Add a new Path-specific Stop</h3>
                    </div>
                    <div class="modal-body">
                        <form class="form-horizontal">
                        <fieldset>
                        <div class="control-group">
                            <label class="control-label" for="addRouteID">ID</label>
                            <div class="controls">
                                <input type="number" id="addRouteID" class="input input-mini" min="0" max="65535"/>
                            </div>
                        </div>
                        <div class="control-group">
                            <label class="control-label" for="addRouteName">Trip Name</label>
                            <div class="controls">
                                <input type="text" id="addRouteName" class="input input-large" maxLength="40"/>
                            </div>
                        </div>
                        <div class="control-group">
                            <label class="control-label" for="addRouteDescription">Stop Text</label>
                            <div class="controls">
                                <textarea rows="2" cols="50" class="input input-xlarge required" id="addRouteDescription" maxLength="200"></textarea>
                            </div>
                        </div>
                        </fieldset>
                        </form>
                    </div>
                    <div class="modal-footer">
                        <button class="btn btn-primary" onclick="addRouteStep2b();">Next</button>
                        <button class="btn" onclick="cancelAddRouteStep1();">Cancel</button>
                    </div>
                </div>

                <div class="semi-modal" id="choosePSSCoordinates" style="display: none; width: 350px;">
                    <div class="modal-header">
                        <h3>Build Route</h3>
                        <p>Select the type of point to add, then hold down the Ctrl key and click at the desired coordinates or enter an address below.</p>
                        <p>The points are added to the route in reverse, so the last point added will be the starting point.</p>
                    </div>
                    <div class="modal-body">
                        <fieldset>
                        <div class="btn-group" data-toggle="buttons-radio">
                            <button class="btn btn-primary active" id="choosingFinalDestination">Destination</button>
                            <button class="btn btn-primary" id="choosingIntermediateDestinations">Intermediate</button>
                            <button class="btn btn-primary" id="choosingShapingPoints">Shaping</button>
                        </div>
                        <div class="destination-info control-group">
                            <label class="control-label" for="destinationName">Name</label>
                            <div class="controls">
                                <input type="text" id="destinationName" class="input input-medium" maxLength="40"/>
                            </div>
                        </div>
                        <div class="shaping-info control-group">
                            <label class="control-label">Name</label>
                            <div class="controls">
                                <span class="input-medium uneditable-input"></span>
                            </div>
                        </div>
                        <div class="control-group">
                            <textarea rows="2" cols="50" class="input input-xlarge required addressField" data-trigger="#locateAddress" id="addressToLocate" placeholder="type a full or partial address here"></textarea>
                            <br/><button class="btn btn-primary" onclick="doLocateAddress();" id="locateAddress" disabled="disabled" data-loading-text="Locating...">Locate Address</button>
                            <div id="newMarkerCoords"></div>
                        </div>
                        </fieldset>
                    </div>
                    <div class="modal-footer" style="width: 320px;">
                        <button class="btn" onclick="commitAddRoutePoints();">Commit</button>
                        <button class="btn" onclick="cancelAddRoutePoints();">Cancel</button>
                    </div>
                </div>

                <div class="semi-modal" id="chooseRouteEstimateOrigin" style="display: none;">
                    <div class="modal-header">
                        <h3>Estimate Route</h3>
                        <h4>Specify Origin</h4>
                        <p>Click at the desired coordinates,<br/> or enter an address below,<br/>or just use the starting point of the route.</p>
                    </div>
                    <div class="modal-body">
                        <input type="hidden" id="estimateRouteToKey"/>
                        <textarea rows="2" cols="40" class="input input-xlarge required addressField" data-trigger="#chooseRouteEstimateOriginFromAddress" id="originAddressToLocate" placeholder="type a full or partial address here"></textarea>
                        <br/>
                        <button class="btn" disabled="disabled" data-loading-text="Locating..." id="chooseRouteEstimateOriginFromAddress">Locate Address</button>
                    </div>
                    <div class="modal-footer">
                        <button class="btn btn-primary" data-loading-text="Locating...">Use first destination</button>
                        <button class="btn" onclick="cancelEstimateRoute();">Cancel</button>
                    </div>
                </div>

                <div class="modal" id="morePSSActions" style="display: none;">
                    <div class="modal-header">
                        <button class="close" data-dismiss="modal">&#215;</button>
                        <h3>Manipulate Path-specific Stop</h3>
                    </div>
                    <div class="modal-body">
                        <form class="form-horizontal">
                        <div class="control-group">
                            <label class="control-label"> </label><button class="btn" onclick="activateStop();">Activate</button>
                        </div>
                        <div class="control-group">
                            <label class="control-label"> </label><button class="btn" onclick="completeStop();">Mark Stop as Done</button>
                        </div>
                        <div class="control-group">
                            <label class="control-label">New position:</label> <input type="number" class="input input-mini" id="newPSSPosition"/>
                            <button class="btn" onclick="repositionStop();">Move Stop</button>
                        </div>
                        <div class="control-group">
                            <label class="control-label"> </label><button class="btn" onclick="deleteRouteFromDevice();">Delete from device</button>
                        </div>
                        <input type="hidden" id="morePSSActionsKey"/>
                        </form>
                    </div>
                </div>

                <script type="text/javascript" src="http://ecn.dev.virtualearth.net/mapcontrol/mapcontrol.ashx?v=7.0"></script>
                <script type="text/javascript" src="../external/mapping/jquery.ui.bmap.full.min.js"></script>
                <script type="text/javascript" src="../external/bootstrap/js/bootstrap.min.js"></script>
                <script type="text/javascript" src="page.js"></script>
                <script type="text/javascript" src="avoidances.js"></script>
                <script type="text/javascript" src="pss.js"></script>
            </body>
        </html>
    </xsl:template>
    <xsl:template match="route" mode="data-elements">
        <li class="route" route-id="{@id}" end-lat="{@end-lat}" end-lon="{@end-lon}">
            <span class="route-name"><xsl:value-of select="@name"/></span>
            <span class="route-descr"><xsl:value-of select="description"/></span>
            <ul class="intermediates">
                <xsl:apply-templates select="intermediate|shaping" mode="data-elements"/>
            </ul>
        </li>
    </xsl:template>
    <xsl:template match="intermediate" mode="data-elements">
        <li inter-id="{position()}" inter-type="intermediate" lat="{@lat}" lon="{@lon}">
            <span class="name"><xsl:value-of select="@name"/></span>
        </li>
    </xsl:template>
    <xsl:template match="shaping" mode="data-elements">
        <li inter-id="{position()}" inter-type="shaping" lat="{@lat}" lon="{@lon}">
            <span class="name"><xsl:value-of select="@name"/></span>
        </li>
    </xsl:template>
</xsl:stylesheet>