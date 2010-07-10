<?php
// $Id: audio_feeds.views.inc,v 1.1 2008/09/25 23:18:08 drewish Exp $

/**
 * @file
 * This file provides various XML feeds for audio views.
 */

function audio_feeds_views_style_plugins() {
  $items = array();
  $items['audio_feeds'] = array(
    'name' => t('Audio: XSPF/M3U/PLS Playlist'),
    'theme' => 'audio_feeds_views_xspf',
    'summary_theme' => 'views_summary',
  );
  return $items;
}

/**
 *  Theme function to handle the XSPF view
 */
function theme_audio_feeds_views_xspf($view, $nodes, $type) {
  if (isset($_GET['xspf'])) {
    return audio_feeds_views_prepare_xspf(views_get_title($view, $type), url($view->real_url, array('absolute' => true)), $nodes);
  }
  elseif (isset($_GET['m3u'])) {
    return theme('audio_feeds_views_m3u', $view, $nodes, $type);
  }
  elseif (isset($_GET['pls'])) {
    return theme('audio_feeds_views_pls', $view, $nodes, $type);
  }
  else {
    $player = audio_get_players('name', variable_get('audio_feeds_default_player', 'xspf_extended'));
    $query_string = array();
    if ($_GET) {
      foreach ($_GET as $key => $value) {
        // filter out the standard drupal q string, leaving everything else
        if ($key != 'q') {
          $query_string[$key] = $value;
        }
      }
    }
    $filters = $query_string ? audio_feeds_query_string_encode($query_string) .'%26xspf' : 'xspf';
    $playlist_url = url($view->real_url, array('query' => $filters, 'absolute' => TRUE));
    $output = theme($player['theme_xspf'], $playlist_url);
    $output .= theme('audio_feeds_views_links', $view, $query_string);
    return $output;
  }
}

/**
 *  Prepare data for XML generation and return the feed
 */
function audio_feeds_views_prepare_xspf($title, $url, $nodes) {
  global $base_url;

  // prepare feed metadata
  $metadata = array(
    'title' => $title,
    'author' => $base_url,
    'link' => $base_url,
    'feed_url' => $url
  );

  // prepare feed items
  foreach ($nodes as $n) {
    $audio = node_load($n->nid);
    // use the first image uploaded as the included image if it exists
    $image = is_array($audio->audio_images) ? current($audio->audio_images) : '';
    $items[] = array(
      'title' => $audio->audio_tags['title'] ? $audio->audio_tags['title'] : $audio->title,
      'author' => $audio->audio_tags['artist'],
      'album' => $audio->audio_tags['album'],
      'duration' => $audio->audio_file['playtime'],
      'link' => url('node/'. $audio->nid, array('absolute' => TRUE)),
      'image' => $image ? array('url' => $base_url .'/'. $image['filepath']) : '',
      'enclosure' => array('url' => $audio->url_play)
    );
  }
  audio_feeds_generate_xspf($items, $metadata);
}


/**
 *  Theme function to handle the M3U view
 */
function theme_audio_feeds_views_m3u($view, $nodes, $type) {
  return audio_feeds_views_prepare_m3u(views_get_title($view, $type), $nodes);
}

/**
 *  Prepare data for XML generation and return the feed
 */
function audio_feeds_views_prepare_m3u($title, $nodes) {

  // prepare feed metadata
  $metadata = array('title' => $title);

  // prepare feed items
  foreach ($nodes as $n) {
    $audio = node_load($n->nid);
    $items[] = array(
      'title' => $audio->audio_tags['title'] ? $audio->audio_tags['title'] : $audio->title,
      'author' => $audio->audio_tags['artist'],
      'duration' => $audio->audio_file['playtime'],
      'enclosure' => array('url' => $audio->url_play)
    );
  }
  audio_feeds_generate_m3u($items, $metadata);
}


/**
 *  Theme function to handle the PLS view
 */
function theme_audio_feeds_views_pls($view, $nodes, $type) {
  return audio_feeds_views_prepare_pls(views_get_title($view, $type), $nodes);
}

/**
 *  Prepare data for XML generation and return the feed
 */
function audio_feeds_views_prepare_pls($title, $nodes) {

  // prepare feed metadata
  $metadata = array('title' => $title);

  // prepare feed items
  foreach ($nodes as $n) {
    $audio = node_load($n->nid);
    $items[] = array(
      'title' => $audio->audio_tags['title'] ? $audio->audio_tags['title'] : $audio->title,
      'author' => $audio->audio_tags['artist'],
      'duration' => $audio->audio_file['playtime'],
      'enclosure' => array('url' => $audio->url_play)
    );
  }
  audio_feeds_generate_pls($items, $metadata);
}

/**
 *  Format the links to the xml feeds for the view
 */
function theme_audio_feeds_views_links($view, $query_string = null) {
  $m3u_filter = $query_string ? drupal_query_string_encode($query_string) .'&m3u' : 'm3u';
  $pls_filter = $query_string ? drupal_query_string_encode($query_string) .'&pls' : 'pls';
  $xspf_filter = $query_string ? drupal_query_string_encode($query_string) .'&xspf' : 'xspf';

  $links = array();
  $links['m3u']['title'] = t('m3u');
  $links['m3u']['href'] = url($view->real_url, array('query' => $m3u_filter, 'absolute' => true));
  $links['pls']['title'] = t('pls');
  $links['pls']['href'] = url($view->real_url, array('query' => $pls_filter, 'absolute' => true));
  $links['xspf']['title'] = t('xspf');
  $links['xspf']['href'] = url($view->real_url, array('query' => $xspf_filter, 'absolute' => true));
  return theme('links', $links);
}


/**
 * Parse an array into a valid urlencoded query string.
 *
 * This function is a work-around for a flash url encode issue with views exposed filters
 * See: http://drupal.org/node/147888 for details.
 *
 * @param $query
 *   The array to be processed e.g. $_GET.
 * @param $exclude
 *   The array filled with keys to be excluded. Use parent[child] to exclude
 *   nested items.
 * @param $parent
 *   Should not be passed, only used in recursive calls.
 * @return
 *   urlencoded string which can be appended to/as the URL query string.
 */
function audio_feeds_query_string_encode($query, $exclude = array(), $parent = '') {
  $params = array();

  foreach ($query as $key => $value) {
    $key = urlencode($key);
    if ($parent) {
      $key = $parent .'['. $key .']';
    }

    if (in_array($key, $exclude)) {
      continue;
    }

    if (is_array($value)) {
      $params[] = audio_feeds_query_string_encode($value, $exclude, $key);
    }
    else {
      $params[] = $key .'='. urlencode($value);
    }
  }

  return implode('%26', $params);
}
