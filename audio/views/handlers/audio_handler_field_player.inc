<?php

/**
 * Show a embedded player for an audio file
 */
class audio_handler_field_player extends views_handler_field {
  function allow_advanced_render() {
    return FALSE;
  }

  function option_definition() {
    $options = parent::option_definition();

    $players = array_keys(audio_get_players('names'));
    $options['player'] = array('default' => $players[0], 'translatable' => FALSE);

    return $options;
  }

  /**
   * Provide link to the page being visited.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    $players = audio_get_players('names');
    $options = array();
    foreach ($players as $name => $player) {
      $options[$name] = $player['title'];
    }
    
    $form['player'] = array(
      '#title' => t('Select a player'),
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => $this->options['player'],
    );
  }

  function render($values) {
    // Don't bother doing a node_load() if there's not a node and file id.
    if (!empty($values->{$this->aliases['nid']}) && !empty($values->{$this->aliases['fid']})) {
      $node = node_load($values->{$this->aliases['nid']});
      return audio_get_node_player($node, $this->options['player']);
    }
  }
}
