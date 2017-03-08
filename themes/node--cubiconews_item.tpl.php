<?php
/*
global $base_url;
if(!empty($base_url)) {
  $base_link_url = $base_url;
} else {
  $base_link_url = 'http://localhost:8060';
}
$var_logoimg = url('themes/bartik/logo.png', array('absolute' => true, 'base_url' => $base_link_url));
$var_logourl = l('<img src="' . $var_logoimg . '" alt="Espressonews.gr Newsletter" title="Espressonews.gr Newsletter">', '<front>', array('html' => true, 'attributes' => array('target' => '_blank')));

$articles = array();
$field_collection_fields = field_get_items('node', $node, 'field_cubiconews_article_group');
foreach ($field_collection_fields as $field_collection_field) {
  $artc = array();

  $field_collection_item = field_collection_item_load($field_collection_field['value']);

  $field_wrapper = entity_metadata_wrapper('field_collection_item', $field_collection_item);
  $artc['title'] = $field_wrapper->field_cubiconews_article_title->value();
  $image_data = $field_wrapper->field_cubiconews_article_image->value();
  $images_style_url = str_replace('http://default/', '', image_style_url('newsletter_thumb', $image_data['uri']));
  $artc['image_url'] = url($images_style_url, array('absolute' => true, 'base_url' => $base_link_url));
  $artc['body'] = $field_wrapper->field_cubiconews_article_body->value()['value'];
  $article_link = $field_wrapper->field_cubiconews_article_link->value();
  $artc['link'] = url($article_link['url'], array('absolute' => true, 'base_url' => $base_link_url));

  $articles[] = $artc;
}
*/
?>
 <iframe src="/newsletter/<?php echo $node->nid; ?>" style="border: 0; width: 100%; height: 900px;"></iframe>
