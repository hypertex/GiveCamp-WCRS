<?php

/**
 * Show a download link to an audio file
 */
class audio_handler_field_download_link extends views_handler_field {
  function allow_advanced_render() {
    return FALSE;
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['text'] = array('default' => 'Download', 'translatable' => TRUE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['text'] = array(
      '#type' => 'textfield',
      '#title' => t('Text to display'),
      '#default_value' => $this->options['text'],
    );
  }

  function render($values) {
    if (!empty($values->{$this->aliases['downloadable']}) && !empty($values->{$this->aliases['nid']}) && !empty($values->{$this->aliases['fid']})) {
      $file = db_fetch_object(db_query("SELECT * FROM {files} WHERE fid=%d", $values->{$this->aliases['fid']}));
      if (isset($file->filepath) && file_exists($file->filepath)) {
        // iTunes and other podcasting programs check the url to determine the
        // file type. we'll add the original file name on to the end. see issues
        // #35398 and #68716 for more info.
        return l($this->options['text'], 'audio/download/'. $values->{$this->aliases['nid']} .'/'. $file->filename, array('absolute' => TRUE));
      }
    }
  }
}
