<?php

function cubiconews_add_node($node){	
	// Get the unsent or new newsletters
	$query = db_select('cubiconews_newsletters', 'cubn')
	        ->fields('cubn', array('nid'))
			->condition('status', 0, '=')
			->orderBy('newsid', 'DESC')
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
				'title' => 'Newsletter - ' . date('d/m/Y'),
				'nid' => $newsletter_nid,
				'status' => 0,
				'created' => time()
			))
			->execute();
	}
	
	
		
	// Insert node values into the active Newsletter field collection
	$entity = entity_create('field_collection_item', array('field_name' => 'field_cubiconews_article_group'));
	
	$entity->field_cubiconews_article_title[LANGUAGE_NONE][] = array('value' => $node->title);
	
	if(isset($node->body[LANGUAGE_NONE][0]['safe_value']) && ($node->body[LANGUAGE_NONE][0]['safe_value'] != '')) {
		$entity->field_cubiconews_article_body[LANGUAGE_NONE][] = array('value' => $node->body[LANGUAGE_NONE][0]['safe_value']);
	}
	
	if(isset($node->field_image['und'])) {
		$file = (object)array(
		  "uid" => 1,
		  "alt" => $node->field_image['und'][0]['alt'],
	  	  "title" => $node->field_image['und'][0]['title'],
		  "uri" => $node->field_image['und'][0]['uri'],
		  "filemime" => file_get_mimetype($node->field_image['und'][0]['uri']),
		  "status" => 1
		);
		$file = file_copy($file, 'public://cubiconews', FILE_EXISTS_RENAME);
		$entity->field_cubiconews_article_image[LANGUAGE_NONE][] = (array)$file;
	}
	$entity->field_cubiconews_article_link[LANGUAGE_NONE][] = array('url' => 'node/' . $node->nid);
	
	$entity->setHostEntity('node', $newsletter_node);
	$entity->save();
	
	
	
	drupal_set_message(t('Node added to newsletter list'), 'status');
	drupal_goto('node/' . $newsletter_nid);

}


function _cubiconews_settings(){
	
	return 'Manage Cubiconews Newsletter System.';
	
}

function _cubiconews_newsletter_list(){
	$output = '<h2>' . t('Manage Cubiconews Newsletters') . '</h2>';
	
	$output .= '<p>' . t('From this page you can see the old newsletters created and create a new one.') . '</p>';
	
	
	$output .= '<table>';
	$output .= '<thead>';
	$output .= '<tr>';
	$output .= '<th>' . t('num') . '</th>';
	$output .= '<th>' . t('Title') . '</th>';
	$output .= '<th>' . t('Created') . '</th>';
	$output .= '<th>' . t('Date sent') . '</th>';
	$output .= '<th>' . t('Status') . '</th>';
	$output .= '<th>' . t('Actions') . '</th>';
	$output .= '</tr>';
	$output .= '</thead>';
	
	$output .= '<tbody>';
		
	$query = db_select('cubiconews_newsletters', 'cubn');
	$query->join('node', 'n', 'cubn.nid = n.nid');
    $query->fields('n', array('nid', 'title', 'created'))
		->fields('cubn', array('status', 'sent'))
		->condition('type', 'cubiconews_item', '=')
        ->orderBy('created', 'ASC');		
	$result = $query->execute();
	
	$cnt = 1;
    while($record = $result->fetchAssoc()) {
		$output .= '<tr>';
		$output .= '<td>' . $cnt . '</td>';
		$output .= '<td>' . l($record['title'], 'node/' . $record['nid']) . '</td>';
		$output .= '<td>' . date('d/m/Y H:i:s', $record['created']) . '</td>';
		$output .= '<td>&nbsp;' . (($record['sent'] > 0) ? date('d/m/Y H:i:s', $record['sent']) : '') . '</td>';
		$output .= '<td>';
		switch($record['status']) {
			case 0:
				$output .= t('New') . '/' . t('Unsent');
				break;
			case 1:
				$output .= t('Ready to send');
				break;
			case 9:
				$output .= t('Sent');
				break;
			case '-1':
				$output .= t('Cancelled');
				break;
		}
		$output .= '</td>';
		$output .= '<td>&nbsp;';
		if($record['status'] == 0) {
			$output .= '<a href="/cubiconews/sendnow/' . $record['nid'] . '" style="margin: 0px 5px;">' . t('Send') . '</a>';
			$output .= '<a href="/cubiconews/removenewsletter/' . $record['nid'] . '" style="margin: 0px 5px;">' . t('Remove') . '</a>';
		}
		$output .= '</td>';
		$output .= '</tr>';
		$cnt++;
	}
	$output .= '</tbody>';
	$output .= '</table>';
	
	return $output;
	
}


function _cubiconews_newsletter_addnew(){
	$output = '<h2>' . t('Create a new Newsletter') . '</h2>';
	
	$output .= '<p>' . t('At the following list you will see the currently selected articles to add to the newsletter. If you would like some of them to be removed, just click the remove link. You can change their order by the order weight field and by pressing the update button.') . '</p>';
	
	$output .= '<form name="newsitems" action="/cubiconews/updorder" method="post">';
	$output .= '<table>';
	$output .= '<thead>';
	$output .= '<tr>';
	$output .= '<th>' . t('num') . '</th>';
	$output .= '<th>' . t('Title') . '</th>';
	$output .= '<th>' . t('Weight') . '</th>';
	$output .= '<th>' . t('Actions') . '</th>';
	$output .= '</tr>';
	$output .= '</thead>';
	
	$output .= '<tbody>';
	
	$query = db_select('cubiconews_nodes', 'cubn');
	$query->join('node', 'n', 'cubn.nid = n.nid');
    $query->fields('n', array('nid', 'title'))
		->fields('cubn', array('cnid', 'cubnid', 'weight'))
		->condition('newsid', 0,'=')
        ->orderBy('weight', 'ASC');
	$result = $query->execute();
	$articles_num = $result->rowCount();
	
	$cubnodes_ids = array();
	$cnt = 1;
    while($record = $result->fetchAssoc()) {
		$output .= '<tr class="' . (($cnt%2 == 0) ? 'even' : 'odd') . '">';
		$output .= '<td>' . $cnt . '</td>';
		$output .= '<td>' . l($record['title'], 'node/' . $record['cubnid']) . '</td>';
		$output .= '<td><input type="text" value="' . $record['weight'] . '" class="fld_order" name="fld-order[]" style="width: 50px;"></td>';
		$output .= '<td>';
		$output .= l(t('Remove'), 'cubiconews/removeitem/' . $record['cnid']) . ' ';
		$output .= l(t('Edit'), 'node/' . $record['cubnid'] . '/edit');
		$output .= '</td>';
		$output .= '</tr>';
		array_push($cubnodes_ids, $record['cnid']);
		$cnt++;
	}

	$output .= '<tr class="even">';
	$output .= '<td>&nbsp;</td>';
	$output .= '<td>&nbsp;</td>';
	
	if($articles_num > 0) {
		$output .= '<td><input type="submit" value="' . t('update order') . '"> <input type="hidden" name="ids_array" value=\'' . serialize($cubnodes_ids) . '\'></td>';
	} else {
		$output .= '<td>&nbsp;</td>';
	}
	$output .= '<td>&nbsp;</td>';
	$output .= '</tr>';
	$output .= '</tbody>';
	$output .= '</table>';
	$output .= '</form>';
	
	if($articles_num > 0) {
		$output .= '<form name="frm_final" action="/cubiconews/finalize" method="post">';
		$output .= '<input type="hidden" name="allids_arr" value=\'' . serialize($cubnodes_ids) . '\'>';
		$output .= '<input type="button" name="btn_preview" id="btn-preview" value="' . t('Preview') . '">';
		$output .= '<input type="submit" name="btn_finalize" id="btn-finalize" value="' . t('Finalize') . '" style="margin: 0px 10px;">';
		$output .= '</form>';
	}
	
	return $output;	
}

function _cubiconews_emails(){
	$output = '<h2>' . t('Manage Newsletter Recipients') . '</h2>';
	
	// Check first if the email already exists in our database
	$count_emails = db_select('cubiconews_emails', 'cube');
	$count_emails->fields('cube', array('status'));
	$count_emails->addExpression('COUNT(*)', 'cnt');
	$count_emails->orderBy('status', 'DESC');
	$count_emails->groupBy('status');
	//$count_emails->countQuery();
	$result = $count_emails->execute();
	
	
	while($record = $result->fetchAssoc()) {
		//print_r($record);
		switch($record['status']) {
			case 0:
				$output .=  '<div class="disabled">' . t('Disabled emails') . ': ' . $record['cnt'] . '</div>';
				break; 
			case 1:
				$output .=  '<div class="enabled">' . t('Enabled emails') . ': ' . $record['cnt'] . '</div>';
				break;			
		}
	}
	$output .=  '<a href="/cubiconews/newsletters/emails/add">' . t('Add new emails') . '</a>';
	return $output;
}

function _cubiconews_emails_add_form($form, &$form_submit){
	$form = array();
	//$form['block_message'] =array(
	//	'#markup' => t('In case you do not want to receive our newsletters anymore, insert your email at the following field to unsubscribe.'),
	//);
	$form['emails_area'] = array(
		'#type' => 'textarea',
		'#title' => t('Emails'),
		'#description' => t('Copy here all the emails you want to import. It doesn\'t matter how the text looks like. The script will recognise the emails and add the new ones in the database. If there are emails that already exist, then these will be enabled again.'),
		'#default_value' => '',
		'#required' => true
	);
	
	$form['btn_submit'] = array(
		'#type' => 'submit',
		'#value' => t('Import'),
		'#name' => 'save',
	);
	
	return $form;
}

function _cubiconews_emails_add_form_submit ($form, &$form_state){
	$emails_address = $form_state['values']['emails_area'];
	$extracted_emails = _extract_emails($emails_address);
	
	$emails = array_unique($extracted_emails[0]);
	$add_cnt = 0;
	$upd_cnt = 0;
	foreach($emails AS $curemail) {
		$ret = _register_email($curemail);
		if($ret == 'new') { // email added in database
			$add_cnt++;
		}
		if($ret == 'upd') { // email updated and enabled
			$upd_cnt++;
		}
	}
	watchdog('cubiconews', '%adds emails added! %upds emails updated!', array('%adds' => $add_cnt, '%upds' => $upd_cnt), WATCHDOG_INFO);
	
	drupal_set_message(t('@adds emails added! @upds emails updated!', array('@adds' =>$add_cnt, '@upds' => $upd_cnt)), 'status');
	drupal_goto('cubiconews/newsletters/emails');
}
