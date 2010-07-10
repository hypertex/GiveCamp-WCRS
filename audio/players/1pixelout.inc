<?php
// $Id: 1pixelout.inc,v 1.4 2007/07/31 03:17:23 drewish Exp $

function audio_1pixelout_audio_player() {
  $items['1pixelout'] = array(
    'module'      => 'audio',
    'title'       => t('1pixelout Flash player'),
    'description' => t('Written by Martin Laine as part of the Wordpress Audio Player plugin.'),
    'url'         => t('http://www.1pixelout.net/code/audio-player-wordpress-plugin/'),
    'preview'     => 'players/1pixelout.png',
    'formats'     => array('wav', 'mp3'),
    'theme_node'  => 'audio_1pixelout_node_player',
  );
  return $items;
}

function theme_audio_1pixelout_node_player($node, $options = array()) {
  // make sure it's compatible with the flash player
  if (!audio_is_flash_playable($node)) {
    return NULL;
  }

  $options['soundFile'] = check_url($node->url_play);
  $url = base_path() . drupal_get_path('module', 'audio') .'/players/1pixelout.swf';
  $flashvars = audio_query_string_encode($options);

$output = <<<EOT
<object type="application/x-shockwave-flash" data="$url" width="290" height="24" >
  <param name="movie" value="$url" />
  <param name="wmode" value="transparent" />
  <param name="menu" value="false" />
  <param name="quality" value="high" />
  <param name="FlashVars" value="$flashvars" />
  <embed src="$url" flashvars="$flashvars" width="290" height="24" />
</object>
EOT;

  return $output;
}
