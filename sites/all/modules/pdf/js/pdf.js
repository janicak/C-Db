(function ($) {
  Drupal.behaviors.pdf = {
    attach: function(context, settings) {
      $('.pdf').each(function(){
        var file = $(this).attr('file');
        window.pdfListView = new PDFListView(
          this,
          {
            textLayerBuilder: TextLayerBuilder,
            logLevel: PDFListView.Logger.DEBUG
          }
        );
        pdfListView.loadPdf(file);
        pdfListView.setScale($(this).attr('scale'));
      });
    }
  };
})(jQuery);
