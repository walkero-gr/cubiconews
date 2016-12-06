<?php
$var_logoimg = url('themes/bartik/logo.png');
$var_logourl = l('<img src="' . $var_logoimg . '" alt="Espressonews.gr Newsletter" title="Espressonews.gr Newsletter">', '<front>', array('html' => true, 'attributes' => array('target' => '_blank')));
?>

<table width="740" border="0" cellpadding="4" cellspacing="0" style="width: 740px;">
	<tr>
	  <td width="285" height="55" bgcolor="#EA4526" style="border-bottom: 4px solid #FFB700;"><?php echo $var_logourl; ?></td>
	  <td align="right" valign="bottom" bgcolor="#EA4526" style="border-bottom: 4px solid #FFB700;">
		<span>
		<a href="http://facebook.com/espressonews.gr" target="_blank"><img src="<?php echo url(drupal_get_path('module', 'cubiconews') . '/facebook.resized.png', array('absolute'=>true)); ?>" border="0" alt="Facebook" title="Βρείτε μας στο Facebook" /></a>&nbsp;
		<a href="http://twitter.com/espressonews_gr" target="_blank"><img src="<?php echo url(drupal_get_path('module', 'cubiconews') . '/twitter.resized.png', array('absolute'=>true)); ?>" border="0" alt="Twitter" title="Βρείτε μας στο Twitter" /></a>&nbsp;
		<a href="http://google.com/+espressonews" target="_blank"><img src="<?php echo url(drupal_get_path('module', 'cubiconews') . '/gplus.resized.png', array('absolute'=>true)); ?>" border="0" alt="Google Plus" title="Βρείτε μας στο Google Plus" /></a>&nbsp;
		<a href="http://youtube.com/espressonewsgr" target="_blank"><img src="<?php echo url(drupal_get_path('module', 'cubiconews') . '/youtube.resized.png', array('absolute'=>true)); ?>" border="0" alt="YouTube" title="Βρείτε μας στο YouTube" /></a>&nbsp;
		</span>
		  <br>
		  <font size="2" face="Arial" color="white"><?php echo t(date('l')) . date(', d / m / Y'); ?></font>
		  <span style="font-size: 11px; color: white;"><br>
		  <a href="http://www.espressonews.gr/taxonomy/term/33" style="color: white;">ΡΕΠΟΡΤΑΖ</a> |
		  <a href="http://www.espressonews.gr/taxonomy/term/1" style="color: white;">ΠΟΛΙΤΙΚΗ</a> |
		  <a href="http://www.espressonews.gr/taxonomy/term/2" style="color: white;">ΚΟΣΜΟΣ</a> |
		  <a href="http://www.espressonews.gr/taxonomy/term/4" style="color: white;">ΥΓΕΙΑ</a> |
		  <a href="http://www.espressonews.gr/taxonomy/term/34" style="color: white;">SHOWBIZ</a>
		  </span>
	  </td>
	</tr>
	<tr>
	  <td colspan="2" style="padding: 5px;" valign="top" class="defaultFont">
		<?php
		
		$field_collection_fields = field_get_items('node', $node, 'field_cubiconews_article_group');
		foreach ($field_collection_fields as $field_collection_field) {
		  $field_collection_item = field_collection_item_load($field_collection_field['value']);
		  
		  $field_wrapper = entity_metadata_wrapper('field_collection_item', $field_collection_item);
		?>
		<p>
			<h3 style="margin: 0px 0px 10px 0px; font-size: 15px; font-family: Arial, Helvetica, sans-serif;"><?php echo $field_wrapper->field_cubiconews_article_title->value(); ?></h3>
			<p>
				<img src="<?php echo image_style_url('newsletter_thumb', $field_wrapper->field_cubiconews_article_image->value()['uri']); ?>" style="border: 6px solid #d7d7d7;">
				<?php echo $field_wrapper->field_cubiconews_article_body->value()['value']; ?>
				<?php echo l(t('Read more'), $field_wrapper->field_cubiconews_article_link->value()['display_url']); ?>
			</p>
		</p>
		<?php 
		} ?>
		
	  </td>
	</tr>
	<tr>
	  <td colspan="2" style="padding: 15px 5px; border-top: 3px solid #EA4526; color: black; font-size: 10px; height: 60px; font-family: Arial, Helvetica, sans-serif;">
		<a href="<?php echo url('node/18', array('absolute'=>true)); ?>" target="_blank">Όροι Χρήσης</a> | <a href="<?php echo url('node/19', array('absolute'=>true)); ?>" target="_blank">Επικοινωνία</a> | <a href="<?php echo url('node/20', array('absolute'=>true)); ?>" target="_blank">Διαφήμιση</a> © 2007-<?php echo date('Y'); ?>, Espresso 
		<p>ΑΠΑΓΟΡΕΥΕΤΑΙ η αναδημοσίευση, η αναπαραγωγή, ολική, μερική ή περιληπτική ή κατά παράφραση ή διασκευή απόδοση του περιεχομένου του παρόντος web site με οποιονδήποτε τρόπο, ηλεκτρονικό, μηχανικό, φωτοτυπικό, ηχογράφησης ή άλλο, χωρίς προηγούμενη γραπτή άδεια του εκδότη.</p>
		<p style="text-align: left;"><?php echo t('If you would like to unsubscribe from our newsletter email list,'); ?> <a href="<?php echo url('cubiconews/registration_remove', array('absolute'=>true)); ?>"><?php echo t('please click here'); ?></a></p>
	  
	  </td>
	</tr>
	<tr>
	  <td colspan="2" style="padding: 0px 5px; height: 40px; font-size: 9px; font-family: Arial, Helvetica, sans-serif; font-style: italic;">* This newsletter is encoded in Unicode (utf8).</td>
	</tr>
</table>



<?php
/*
echo '<hr>';
print_r($node);
echo '<hr>';


    $field_collection_fields = field_get_items('node', $node, 'field_cubiconews_article_group');
	foreach ($field_collection_fields as $field_collection_field) {
	  $field_collection_item = field_collection_item_load($field_collection_field['value']);
	  
	  $field_wrapper = entity_metadata_wrapper('field_collection_item', $field_collection_item);
	  echo $field_wrapper->field_cubiconews_article_title->value();
	}
echo '<hr>';
*/
?>
