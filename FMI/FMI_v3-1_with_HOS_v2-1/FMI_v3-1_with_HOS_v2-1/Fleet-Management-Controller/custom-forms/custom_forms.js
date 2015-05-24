/**
 * navigation for custom form template rendering as HTML form
 */

// show the fieldset for the selected form item
function jumpTo(target,e) {
    jQuery('#tableOfContents').hide();
    jQuery(target).addClass('editing');
}
// copy the item's field selected value to the read-only rendering
function saveSelection(e) {
    var container = jQuery(e.target).closest('fieldset');
    var val = jQuery('input',container).val();
    switch (jQuery('input',container).attr('type')) {
        case 'checkbox':
        case 'radio':
            // join them all (radio will just get one)
            var texts = [];
            jQuery('input:checked',container).each(function(idx,item) {
                texts[texts.length] = jQuery(item.parentNode).text();
            });
            if (texts.length == 0) {
                return false;
            }
            val = texts.join(',');
            break;
        default:
            if (val == null || jQuery.trim(val) == '') {
                return false;
            }
    }
    jQuery('#toc-'+container.attr('id')+' .item-value').text(val);
    return true;
}
// hide the fieldset for the selected form item and return to the table of contents
function toTOC(e) {
    jQuery('form fieldset').removeClass('editing');
    jQuery('#tableOfContents').show();
    document.location.hash = '';
    return false;
}
// initialize special widgets and navigation
function initializeForm() {
    jQuery(function() {
        jQuery('input.date').datepicker();
        jQuery('input.time').timepicker();

        jQuery('.btn-cancel').on('click', function(e) {
            e.preventDefault();
            return toTOC(e);
        });
        jQuery('.btn-primary').on('click', function(e) {
            e.preventDefault();
            if (saveSelection(e)) return toTOC(e);
        });
    });
}