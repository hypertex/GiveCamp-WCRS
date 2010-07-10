<?php
// $Id: button.inc,v 1.7 2008/01/23 04:35:00 drewish Exp $

function audio_button_audio_player() {
  $items['button'] = array(
    'module'      => 'audio',
    'title'       => t('XSPF Web Music Player - Button'),
    'description' => t('Cute button player...'),
    'url'         => t('http://musicplayer.sourceforge.net/'),
    'preview'     => 'players/button.png',
    'formats'     => array('wav', 'mp3'),
    'theme_node'  => 'audio_button_node_player',
  );
  return $items;
}

/**
 * The default mp3 player is the flash XSPF player. If you want to override
 * this behavior you can do so at the theme level.
 *
 * A couple of options that may be of interest:
 * - backgroud color. If it's not specified, it'll default to transparent.
 *    'b_bgcolor' => "000000"
 * - foregroung (border and icon) color.
 *    'b_fgcolor' => "ffffff"
 * - foreground color by state. The order is: connecting (spinner),
 *   ready (arrow), playing (square), and ??? (not sure). The b_fgcolor value
 *   will be used for any ommitted values.
 *    'b_colors' => "ff0000,,00000ff,000000",
 *
 * @param $node
 *   node object
 * @param $options
 *   array of options to pass to the player. This allows you to override the
 *   any parameter passed to the player including colors, title,
 * @return
 *   HTML string with a flash player
 */
function theme_audio_button_node_player($node, $options = array()) {
  // make sure it's compatible with the flash player
  if (!audio_is_flash_playable($node)) {
    return NULL;
  }

  $options['song_url'] = check_url($node->url_play);
  $options['song_title'] = check_plain($node->audio_tags['title']);

  $url = base_path() . drupal_get_path('module', 'audio') .'/players/button.swf';
  $flashvars = audio_query_string_encode($options);

$output = <<<EOT
<object type="application/x-shockwave-flash" data="$url" width="17" height="17">
  <param name="movie" value="$url" />
  <param name="wmode" value="transparent" />
  <param name="quality" value="high" />
  <param name="FlashVars" value="$flashvars" />
  <embed src="$url" flashvars="$flashvars" width="17" height="17" />
</object>
EOT;

  return $output;
}
