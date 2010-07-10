<?php

/**
 * Show a embedded player for an audio file
 */
class audio_handler_field_play_link extends views_handler_field {
  function allow_advanced_render() {
    return FALSE;
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['text'] = array('default' => 'Play', 'translatable' => TRUE);
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
    if (!empty($this->aliases['nid']) && !empty($this->aliases['fid'])) {
      if (!empty($values->{$this->aliases['fid']})) {
        return l($this->options['text'], 'audio/play/'. $values->{$this->aliases['nid']}, array('absolute' => TRUE));
      }
    }
    return '';
  }
}
