Drupal.behaviors.chosen_facetFacetApiMultiselectWidget = {
  attach: function (context, settings) {
    // Go through each facet API multiselect element that is being displayed.
    jQuery('.facetapi-multiselect', context).each(function () {
      // Attach the behavior to it.
      jQuery(this).chosen({
        disable_search_threshold: 10,
        no_results_text: "Oops, nothing found!",
        width: "95%"
      });
    });
  }
};