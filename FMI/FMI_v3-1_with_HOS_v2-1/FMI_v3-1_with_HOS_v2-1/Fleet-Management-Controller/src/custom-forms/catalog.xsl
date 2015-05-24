<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:msxsl="urn:schemas-microsoft-com:xslt"
    xmlns:date="urn:date"
    exclude-result-prefixes="msxsl date">
    <xsl:output method="html" encoding="utf-8" indent="yes" />
    <!-- this script introduces a function that can be called during XSL transformation to format timestamps -->
    <msxsl:script implements-prefix="date" language="JavaScript">
    <![CDATA[
      function FormatDateTime(seconds)
      {
          var d = new Date(seconds*1000);
          return (d.getMonth()+1) + '/' + d.getDate() + '/' + d.getFullYear() + ' ' + d.toLocaleTimeString();
      }
    ]]>
    </msxsl:script>
    <xsl:template match="catalog">
        <xsl:text disable-output-escaping="yes">&lt;!DOCTYPE html></xsl:text>
        <html>
            <head>
                <title>Custom Forms Library</title>
                <link href="../external/bootstrap/css/bootstrap.min.css" rel="stylesheet" media="screen"/>
                <style type="text/css">
                body {
                    font-family: Arial;
                }
                h2 {
                    border-bottom: 1px solid black;
                }
                h3 {
                    border-bottom: 1px solid lightgrey;
                }
                ul.submits {
                    list-style-type: none;
                }
                button {
                    margin-left: 0.5em;
                }
                .file-title>* {
                    float: left;
                    margin-right: 0.5em;
                    line-height: 20px;
                }
                a.parse-error {
                    position: relative;
                }
                a.parse-error>div {
                    position: absolute;
                    width: 500px;
                    left: -100px;
                    bottom: 0.9em;
                    display: none;
                    background-color: #f6eab0;
                    border: 1px solid gray;
                    padding: 1em;
                }
                ul.submits a.parse-error>div {
                    left: -250px;
                }
                a.parse-error:hover>div {
                    display: block;
                }
                </style>
                <!-- these functions interact with the hosting application and will not work in a regular browser -->
                <script type="text/javascript">
                    function importForm() {
                        location.href = 'app:importForm()';
                    }
                    function removeForm(path) {
                        location.href = 'app:removeForm("'+path+'")';
                    }
                    function deleteCustom() {
                        while (true) {
                            var id = prompt('enter in the form template id on the device to delete (numeric characters only)', '');
                            if (id == null) {
                                return;
                            }
                            if (parseInt(id) == id) {
                                location.href = 'app:deleteCustom('+id+')';
                                return;
                            }
                        }
                    }
                    function sendForm(target) {
                        location.href = 'app:sendForm("' + target + '")';
                    }
                    function deleteForm(id) {
                        location.href = 'app:deleteCustom(' + id + ')';
                    }
                    function changePosition(id,current) {
                        while (true) {
                            var newPos = prompt('specify a new position (numeric characters only, 1-256)', current);
                            if (newPos == null) {
                                return;
                            }
                            if (parseInt(newPos) == newPos) {
                                location.href = 'app:changePosition('+id+','+newPos+')';
                                return;
                            }
                        }
                    }
                    function requestPosition(id) {
                        location.href = 'app:requestPosition('+id+')';
                    }
                </script>
            </head>
            <body>
                <button class="btn" onclick="importForm();">Import Form Template</button>
                <button class="btn" onclick="deleteCustom();">Send Custom Delete command</button>
                <h1>Catalog</h1>
                <xsl:apply-templates select="form"/>
                <script src="../external/jquery-1.7.2.min.js"></script>
                <!-- this script is necessary because MSXML likes to URL Encode anything that is transformed into an anchor's href attribute - very strange -->
                <script type="text/javascript">
                    jQuery(function() {
                        jQuery('a.preview').each(function(idx,item) {
                            item = jQuery(item);
                            item.attr('href',item.attr('data-href'));
                        });
                    });
                </script>
            </body>
        </html>
    </xsl:template>
    <!-- list out each "form", which is just a folder name, containing template version's and associated submitted forms -->
    <xsl:template match="form">
        <h2>form <xsl:value-of select="@id"/></h2>
        <div class="versions"><xsl:apply-templates select="version"/></div>
    </xsl:template>
    <xsl:template match="version">
        <xsl:choose>
        <!-- if template file exists, display the id and position as embedded in the file itself, which may not match what is on the device -->
        <!-- include actions for manipulating the form in the catalog and on the device -->
        <xsl:when test="template">
            <xsl:variable name="tmplt" select="document(template/url)/form"/>
            <div class="file-title">
                <h3>Version <xsl:value-of select="@id"/></h3>
                <button class="btn" onclick="sendForm('{template/url}');">Send</button>
                <button class="btn" onclick="deleteForm({$tmplt/@id});">Delete</button>
                <button class="btn" onclick="removeForm('{template/url}');">Remove</button>
            </div>
            <div style="clear:both;"></div>
            title: <xsl:value-of select="$tmplt/title"/>
            &#160;<a data-href="template:{template/url}" class="preview template">(preview)</a>
                <xsl:if test="template/parseError">
                    <a class="parse-error" title="invalid XML file">
                        <img src="../external/famfamfam/silk/icons/error.png"/>
                        <div>
                            <div>line <xsl:value-of select="template/parseError/@line"/>,
                                 character <xsl:value-of select="template/parseError/@char"/>:
                                 <xsl:value-of select="template/parseError"/>
                            </div>
                        </div>
                    </a>
                </xsl:if>
            <br/>id: <xsl:value-of select="$tmplt/@id"/>
            <br/>position: <xsl:value-of select="$tmplt/@position"/><button class="btn" onclick="requestPosition({$tmplt/@id});">Query</button><button class="btn" onclick="changePosition({$tmplt/@id},{$tmplt/@position});">Change</button>
        </xsl:when>
        <!-- else if no template file exists -->
        <xsl:otherwise>
            <h3>Version <xsl:value-of select="@id"/> (template file not found)</h3>
        </xsl:otherwise>
        </xsl:choose>
        <h4>Submits</h4>
        <ul class="submits"><xsl:apply-templates select="submit"/></ul>
    </xsl:template>
    <!-- display submitted form by id along with date submitted, and a preview link -->
    <xsl:template match="submit">
        <li>
            <xsl:variable name="submitted" select="document(./url)/form"/>
            <xsl:value-of select="$submitted/@submit-id"/> on <xsl:value-of select="date:FormatDateTime(number($submitted/@submit-time))"/>
            &#160;<a data-href="submitted:{./url}" class="preview submitted">(preview)</a>
            <xsl:if test="./parseError">
                &#160;
                <a class="parse-error" title="invalid XML file">
                    <img src="../external/famfamfam/silk/icons/error.png"/>
                    <div>
                        <div>line <xsl:value-of select="./parseError/@line"/>,
                             character <xsl:value-of select="./parseError/@char"/>:
                             <xsl:value-of select="./parseError"/>
                        </div>
                    </div>
                </a>
            </xsl:if>
        </li>
    </xsl:template>
</xsl:stylesheet>