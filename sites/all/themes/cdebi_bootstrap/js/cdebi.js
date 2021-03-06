Drupal.behaviors.cdebiBootstrapBehaviors = {
  attach: function (context, settings) {

    //Add scrollbar plugin for Colorboxes
    /**jQuery(document).bind('cbox_complete', (function($) {
        $('#cboxContent > #cboxLoadedContent').mCustomScrollbar({
            theme:"dark",
            scrollButtons:{
              enable: true,
              scrollSpeed: 500
            }
        });
        $('.mCSB_container').mCustomScrollbar("destroy");

    })(jQuery));**/

  	jQuery(document).ready(function($) {

  			// Remove default styling from Chosen selects
  			$('div.chosen-container').removeClass('form-control').removeAttr('style');
  			
        // Remove size attribute from fulltext search input
        $('div.views-widget > div > div > input#edit-fulltext').removeAttr('size').removeAttr('maxlength');

        // Move Taxonomy Term Edit Parents below Title edit
        $('div.form-item-parent').insertAfter('.form-item-name');

        // Provide the HTML to create the views mode modal dialog in the Bootstrap style.
          Drupal.theme.prototype.ViewModeModal = function () {
            var html = ''
            html += '  <div id="ctools-modal" class="modal fade" tabindex="-1" role="dialog" aria-labelledby="ctools-modal" aria-hidden="false" style="display: block;">'
            html += '    <div class="ctools-modal-dialog modal-dialog">'
            html += '      <div class="modal-content">'
            html += '        <div class="modal-header">';
            html += '          <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>';
            html += '          <h4 id="modal-title" class="modal-title">&nbsp;</h4>';
            html += '        </div>';
            html += '        <div id="modal-content" class="modal-body">';
            html += '        </div>';
            html += '      </div>';
            html += '    </div>';
            html += '  </div>';

            return html;
          }

        // Apply button classes to view mode modal link
        $('div.field-name-view-details > div > div').addClass('btn btn-xs btn-success');

        /** Apply button classes to edit link
        $('.views-field-view-node a.colorbox-node').addClass('btn btn-xs btn-default').attr("rel", "cbox");
        $('.views-field-view-node a:last-child').addClass('btn btn-xs btn-primary');
        $('.views-field-field-urls a').addClass('btn btn-xs btn-primary');
        $('.views-field-edit-node a').addClass('btn btn-xs btn-default');**/

        //jQuery TreeView
        $('#facetapi-facet-search-apipublications-index-block-field-locationsparents-all, #facetapi-facet-search-apipublications-index-block-field-programs-and-eventsfield-program-or-eventparents-all, #facetapi-facet-search-apipublications-index-block-field-methods-and-toolsfield-method-or-toolparents-all, #facetapi-facet-search-apidatasets-index-block-field-programs-and-eventsfield-program-or-eventparents-all, #facetapi-facet-search-apidatasets-index-block-field-methods-and-toolsfield-method-or-toolparents-all, #facetapi-facet-search-apidatasets-index-block-field-locationsparents-all, #facetapi-facet-search-apidatasets-index-block-field-locationsparents-all, #facetapi-facet-search-apidatasets-index-block-field-data-typeparents-all')
        .each( function(index, element) {
          if (!$(this).hasClass("treeview")) {
            var first = $(this).children("li:first-child");
            var siblings = first.siblings().val();
            if (siblings == '') {
              $(this).treeview({
                collapsed: true,
                unique: false,
                persist: "location",
              });
            } else {
              $(this).treeview({
                collapsed: false,
                unique: false,
                persist: "location",
              });
            }
          }
        });

        
        //Add Bootstrap table classes to views tables
        //.match(/^[a-zA-Z0-9]+/)
        $('.view-datasets-index table.views-table').addClass('table-bordered table-striped');
        $('#block-system-main button.btn-info').removeClass('btn-info').addClass('btn-primary');
        $('.view-publications .views-table').removeClass('table-bordered table-striped').addClass('table-condensed table');
        $('.panel-collapse.abstract-panel').each( function(index, element) {
          var children = $(this).children('div.panel-body').children();
          if (children.length === 0) {
            $(this).parents('div.panel-default').empty();
          }
        });
        $('.panel-collapse.meta-panel').each( function(index, element) {
          var children = $(this).children('div.panel-body').children().children();
          if (children.length === 0) {
            $(this).parents('div.panel-default').empty();
          }
        });        
        $('.panel-collapse.files-panel').each( function(index, element) {
          var childfiles = $(this).children('table');
          var childimages = $(this).children('.view-content div').children();
          var children = childfiles.length + childimages.length;
          if (children === 1) {
            $(this).parents('div.panel-default').empty();
          }
        }); 
        $('.panel-default').each (function(index, element) {
          var children = $(this).children();
          if (children.length === 0) {
            $(this).remove();
          }
        });

        //Abbreviated author list for publication result search header
        $('.views-field-biblio-authors span').each (function(index, element){
          var authors = $(this).text().split('; ');
          if (authors.length > 2){
            $(this).html(authors[0] + " <em>et al.</em>");
          } else if (authors.length == 2) {
            $(this).html(authors[0] + " & " + authors[1]);
          } else if (authors.length == 1) {
            $(this).html(authors[0]);
          }

        });

        //Add Bootstrap classes to main menu nav
        $('#block-system-main-menu > ul').addClass('nav-pills');

        //Add Filter label above facets, results label above results
        $('div.region-sidebar-first section.block-facetapi:first-child').before('<label class="lead">Filter</label>');
        //$('.page-publications #block-system-main .view-publications-index > div.view-content, .page-datasets #block-system-main div.view-content table:first-child').before('<label class="lead">Results</label>');
        $('div.view-header').addClass('lead').insertAfter('.view-filters');
        $('label[for=edit-fulltext]').addClass('lead').css('font-weight', '200');
        $('label[for=edit-sort-bef-combine]').addClass('lead').css('font-weight', '200');

        //Replace data type taxonomy paths with search queries on dataset searches
        $('div.view-datasets-content- .views-field-field-data-type-1 a').each ( function(index, element) {
          var urlString=$(this).attr('href');
          var splitUrlArray = urlString.split('/'); 
          var lastPart = splitUrlArray.pop().replace(/-/g, ' ');
          link = "/datasets?fulltext=" + lastPart;
          $(this).attr('href', link);
        });

        //Add custom scrollbar plugin for horizontal image layout in Publication search
        $(window).load(function() {
            $('div.image-thumbnails > span.field-content').mCustomScrollbar({
                horizontalScroll:true,
                theme:"dark",
                scrollButtons:{
                  enable: true,
                  scrollSpeed: 500
                }
            });
        });

        //Move Entity Connect button div after input field
        /**$('.form-autocomplete').each( function (index, element) {
          $(this).children('.entityconnect-edit').insertBefore(this);
          $(this).children('.entityconnect-add').insertBefore(this);
        });**/

        //Remove data-toggle from main menu links
        $('#block-system-main-menu a')
          .removeAttr('data-toggle')
          .attr('data-hover', 'dropdown')
          .removeAttr('data-target')
          .removeClass('dropdown-toggle');

        //Remove overflow property from vertical tabsets to accomodate autocomplete dropdown
        $('.tabs-left > .tab-content, .tabs-right > .tab-content').css("overflow", "");
        
        


  	});

    }



};