<?php

// $Id: audio_import.pages.inc,v 1.6 2009/05/14 19:13:23 drewish Exp $


function audio_import_validate_file($file) {
  $errors = array();
  // TODO add in some validation to ensure that we only import audio files...
  return $errors;
}

function audio_import_form() {
  $form = array();

  $dirpath = variable_get('audio_import_path', '');
  if (!file_check_directory($dirpath)) {
    drupal_set_message(t("You need to configure the import directory on the audio import module's <a href='!settings-page'>settings page</a>.", array('!settings-page' => url('admin/settings/audio_import'))), 'error');
    return $form;
  }

  $form['#dirpath'] = $dirpath;
  $form['#node_type'] = 'audio';

  // convert the extensions setting into a regex for file scanning
  $extensions = variable_get('audio_allowed_extensions', 'mp3 wav ogg');
  $extensions = preg_split('/[\s,]+/', $extensions, -1, PREG_SPLIT_NO_EMPTY);
  $filemask = '.*(\.'. implode('|\.', $extensions) .')$';

  $files = file_scan_directory($dirpath, $filemask);
  ksort($files);

  // When the form gets too large we end up running out of memory submitting it.
  // To avoid this we use a pager and rather than setting up all the variables
  // ourself we just send in a fake query and then select the desired part of
  // the files array.
  $page_size = variable_get('audio_import_page_size', 50);
  pager_query('SELECT %d', $page_size, 0, 'SELECT %d', array(count($files)));
  $files = array_slice($files, $GLOBALS['pager_page_array'][0] * $page_size, $page_size);

  if ($files) {
    if (module_exists('taxonomy')) {
      // here's a little hack to get the taxonomy controls onto our form
      $form['type'] = array('#type' => 'value', '#value' => $form['#node_type']);
      $form['#node'] = new stdClass();
      $form['#node']->type = $form['#node_type'];
      taxonomy_form_alter($form, array(), $form['#node_type'] .'_node_form');
      unset($form['type']);
      unset($form['#node']);
    }

    $form['token_help'] = array(
      '#title' => t('Token list'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => t('This is a list of the tokens that can be used in the title of audio nodes.'),
      'help' => array('#value' => theme('token_help', 'node')),
    );

    // Put the image files into an array for the checkboxes and gather
    // additional information like dimensions and filesizes. Make sure that
    // there's no 0th element, because a checkbox with a zero value is seen as
    // unchecked and won't be imported.
    $index = 0;
    foreach ($files as $file) {
      $index++;
      $filelist[$index] = substr($file->filename, strlen($dirpath) + 1);

      // Spit out the import form elements.
      $form['files']['import'][$index] = array(
        '#type' => 'checkbox',
        '#title' => substr($file->filename, strlen($dirpath) + 1),
      );
      $form['files']['filesize'][$index] = array(
        '#type' => 'item',
        '#value' => format_size(filesize($file->filename)),
      );
      $form['files']['title'][$index] = array(
        '#type' => 'textfield',
        '#size' => 20,
        '#default_value' => variable_get('audio_default_title_format', '[audio-tag-title-raw] by [audio-tag-artist-raw]'), // basename($file->name),
      );
      $form['files']['body'][$index] = array(
        '#type' => 'textfield',
        '#size' => 20,
      );

      // If there were problems don't let them import it
      $problems = audio_import_validate_file($file);
      if (count($problems)) {
        $form['files']['import'][$index]['#type'] = 'item';
        $form['files']['errors'][$index] = array(
          '#type' => 'markup',
          '#value' => '<em>'. implode(' ', $problems) .'</em>',
        );
        unset($form['files']['title'][$index]);
        unset($form['files']['body'][$index]);
      }
    }

    $form['pager'] = array('#value' => theme('pager', NULL, $page_size, 0));

    // Put the titles into an array.
    $form['files']['import']['#tree'] = TRUE;
    $form['files']['title']['#tree'] = TRUE;
    $form['files']['body']['#tree'] = TRUE;

    // Store a copy of the list into a form value so we can compare it to what
    // they submit and not have to worry about files being added or removed
    // from the filesystem.
    $form['file_list'] = array(
      '#type' => 'value',
      '#value' => $filelist,
    );

    $form['buttons']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Import'),
    );
  }
  else {
    $form['none_found'] = array(
      '#type' => 'item',
      '#value' => t('<em>No files were found.</em>'),
    );
  }

  return $form;
}

function theme_audio_import_form($form) {
  $output = drupal_render($form['token_help']);

  if (!empty($form['file_list']['#value'])) {
    $type = node_get_types('type', $form['#node_type']);
    $header = array(theme('table_select_header_cell'), t('Name'), t('Size'), check_plain($type->title_label), check_plain($type->body_label));
    $rows = array();
    foreach (element_children($form['files']['import']) as $key) {
      $filename = $form['files']['import'][$key]['#title'];
      unset($form['files']['import'][$key]['#title']);
      $row = array(
        array('data' => drupal_render($form['files']['import'][$key])),
        array('data' => $filename),
        array('data' => drupal_render($form['files']['filesize'][$key])),
      );
      if (!isset($form['files']['errors'][$key])) {
        $row[] = array('data' => drupal_render($form['files']['title'][$key]));
        $row[] = array('data' => drupal_render($form['files']['body'][$key]));
      }
      else {
        $row[] = array('colspan' => 2, 'data' => drupal_render($form['files']['errors'][$key]));
      }

      $rows[] = $row;
    }
    $output .= theme('table', $header, $rows);
  }
  return $output . drupal_render($form);
}

function audio_import_form_submit($form, &$form_state) {
  $batch = array(
    'title' => t('Importing audio'),
    'progress_message' => 'Importing @current of @total.',
    'operations' => array(),
    'finished' => '_audio_import_batch_finished',
    'file' => drupal_get_path('module', 'audio_import') .'/audio_import.pages.inc',
  );

  foreach (array_filter($form_state['values']['import']) as $index => $true) {
    $origname = $form_state['values']['file_list'][$index];
    if ($filepath = file_check_location($form['#dirpath'] .'/'. $origname, $form['#dirpath'])) {
      $args = array(
        'node_type' => $form['#node_type'],
        'title' => isset($form_state['values']['title'][$index]) ? $form_state['values']['title'][$index] : NULL,
        'body' => isset($form_state['values']['body'][$index]) ? $form_state['values']['body'][$index] : NULL,
        'taxonomy' => isset($form_state['values']['taxonomy']) ? $form_state['values']['taxonomy'] : array(),
        'filepath' => $filepath,
        'origname' => $origname,
      );
      $batch['operations'][] = array('_audio_import_batch_op', array($args));
    }
  }

  batch_set($batch);
}

function _audio_import_batch_op($args, &$context) {
  // Create the node object.
  if ($node = audio_api_insert($args['filepath'], $args['title'], $args['body'], array(), $args['taxonomy'])) {
    // Remove the original image now that the import has completed.
    file_delete($args['filepath']);

    $context['results']['good'][] = t('Imported %origname as <a href="!node-link">@node-title</a> @status <a href="!edit-link">[edit]</a>', array(
      '%origname' => $args['origname'],
      '!node-link' => url('node/'. $node->nid),
      '@node-title' => $node->title,
      '@status' => $node->status ? '' : t('(Unpublished)'),
      '!edit-link' => url('node/'. $node->nid .'/edit'),
    ));
  }
  else {
    watchdog('audio_import', 'There was an error that prevented %filename from being imported.', array('%filename' => $args['filepath']), WATCHDOG_ERROR);
    $context['results']['bad'][] = t('Error importing %filename.', array('%filename' => $args['filepath']));
  }

  $context['finished'] = 1;
}

function _audio_import_batch_finished($success, $results, $operations) {
  if (!$success) {
    if (count($results['bad'])) {
      drupal_set_message(t('There was a problem importing files: !bad-list', array('!bad-list' => theme('item_list', $results['bad']))), 'error');
    }
    else {
      drupal_set_message(t('There was a problem importing the files.'), 'error');
    }
  }
  if (count($results['good'])) {
    drupal_set_message(t('Successfully imported: !good-list', array('!good-list' => theme('item_list', $results['good']))));
  }
  watchdog('audio_import', 'Completed audio import.');
}
