<?php

function cubiconews_add_node($sourceNode){
	// Get the unsent or new newsletters
	$query = db_select('cubiconews_newsletters', 'cubn')
    ->fields('cubn', array('nid'))
    ->condition('status', 0, '=')
    ->orderBy('nlid', 'DESC')
    ->range(0,1);
	$result = $query->execute();
	$record = $result->fetchAssoc();

	if(isset($record['nid'])) {
		// Available newsletter exists
		$newsletter_nid = $record['nid'];
		$newsletter_node = node_load($newsletter_nid);
	} else {
		// no newsletter available
		$newNode = new StdClass();
		$newNode->type = 'cubiconews_item';
		$newNode->uid = 1;
		$newNode->status = 1;
		$newNode->comment = 0;
		$newNode->promote = 0;
		$newNode->moderate = 0;
		$newNode->sticky = 0;
		$newNode->language = 'und';
		$newNode->title = 'Newsletter - ' . date('d/m/Y');

		node_object_prepare($newNode);
		node_submit($newNode);
		node_save($newNode);

		$newsletter_nid = $newNode->nid;
		$newsletter_node = $newNode;

		$nid = db_insert('cubiconews_newsletters')
			->fields(array(
				'nid' => $newsletter_nid,
				'status' => 0,
			))
			->execute();
	}


	// Insert node values into the active Newsletter field collection
	$entity = entity_create('field_collection_item', array('field_name' => 'field_cubiconews_article_group'));

	$entity->field_cubiconews_article_title[LANGUAGE_NONE][] = array('value' => $sourceNode->title);

  if(!empty($sourceNode->body[LANGUAGE_NONE][0]['safe_value'])) {
		$entity->field_cubiconews_article_body[LANGUAGE_NONE][] = array('value' => $sourceNode->body[LANGUAGE_NONE][0]['safe_value']);
	}

	if(isset($sourceNode->field_image['und'])) {
		$file = (object)array(
		  "uid" => 1,
		  "alt" => $sourceNode->field_image['und'][0]['alt'],
	  	"title" => $sourceNode->field_image['und'][0]['title'],
		  "uri" => $sourceNode->field_image['und'][0]['uri'],
		  "filemime" => file_get_mimetype($sourceNode->field_image['und'][0]['uri']),
		  "status" => 1
		);
		$file = file_copy($file, 'public://cubiconews', FILE_EXISTS_RENAME);
		$entity->field_cubiconews_article_image[LANGUAGE_NONE][] = (array)$file;
	}
	$entity->field_cubiconews_article_link[LANGUAGE_NONE][] = array('url' => 'node/' . $sourceNode->nid);

	$entity->setHostEntity('node', $newsletter_node);
	$entity->save();

	drupal_set_message(t('Node added to newsletter list'), 'status');
	drupal_goto('node/' . $newsletter_nid);
}
