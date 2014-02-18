<section id="<?php print $block_html_id; ?>" class="<?php print $classes; ?> clearfix panel panel-default"<?php print $attributes; ?>>

  <?php print render($title_prefix); ?>
  <?php if ($title): ?>
  <div class="panel-heading"> <?php print $title; ?></div>
  <?php endif;?>
  <?php print render($title_suffix); ?>
  <?php print $content ?>

</section> <!-- /.block -->
