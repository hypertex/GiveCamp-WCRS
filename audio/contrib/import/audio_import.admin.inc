<?php

// $Id: audio_import.admin.inc,v 1.3 2009/05/10 19:15:23 drewish Exp $

function audio_import_admin_settings() {
  $form['audio_import_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Import path'),
    '#default_value' => variable_get('audio_import_path', file_directory_temp() .'/audio/'),
    '#after_build' => array('_audio_import_settings_check_directory'),
    '#description' => t("The directory to import audio nodes from. Drupal will need to have write access to this directory so we can move the file.") .'<br />'
      . t("<strong>Note:</strong> a path begining with a <kbd>/</kbd> indicates the path is relative to the server's root, one starting without specifies a path relative to Drupal's root. I.e. <kbd>/tmp/audio</kbd> would be the temp directory off the root while <kbd>tmp/audio</kbd> would be inside Drupal's directory."),
    '#required' => TRUE,
  );
  return system_settings_form($form);
}

/**
 * Checks the existence of the directory specified in $form_element.
 *
 * @param $form_element
 *   The form element containing the name of the directory to check.
 * @see system_check_directory()
 */
function _audio_import_settings_check_directory($form_element) {
  $import_dir = $form_element['#value'];
  file_check_directory($import_dir, 0, $form_element['#parents'][0]);
  $audio_dir = variable_get('audio_file_path', file_directory_path() .'/audio');
  if (realpath($import_dir) == realpath($audio_dir)) {
    form_set_error($form_element['#parents'][0], t("You can't import from the audio module's directory. The import deletes the original files so you would just be asking for trouble."));
  }
  else {
    drupal_set_message(t("Your settings are configured correctly, you can import audio <a href='!audio_import_page'>here</a>.", array('!audio_import_page' => url('admin/content/audio_import'))));
  }

  return $form_element;
}
