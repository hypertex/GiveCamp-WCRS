<?php
// $Id: audio.pages.inc,v 1.2 2008/10/02 18:03:44 drewish Exp $


/**
 * Provide a way to browse the audio by metadata.
 *
 * @param $tag
 *   ID3 tag name.
 * @param $value
 *   Value of the tag. It'll be "cleaned" of non-alphanumeric values characters
 *   comparing it.
 */
function audio_page_browse_by($tag = NULL, $value = NULL) {
  $output = '';
  $breadcrumb = array(l(t('Audio'), 'audio'));

  if (isset($tag)) {
    $breadcrumb[] = l(t('Browse by'), 'audio/by');

    if (isset($value)) {
      $breadcrumb[] = l(t('@tag', array('@tag' => $tag)), 'audio/by/'. $tag);

      $result = db_query(db_rewrite_sql("SELECT n.nid, n.title, a.tag, a.value FROM {node} n INNER JOIN {audio_metadata} a ON n.vid = a.vid WHERE a.tag = '%s' AND a.clean = '%s' AND n.status = 1 ORDER BY n.created ASC"), $tag, audio_clean_tag($value));
      while ($obj = db_fetch_object($result)) {
        if ($node = node_load($obj->nid)) {
          $output .= node_view($node, TRUE);
        }
      }
    }
    else {
      $items = array();
      $result = db_query(db_rewrite_sql("SELECT a.value, a.clean, COUNT(a.clean) AS count FROM {node} n INNER JOIN {audio_metadata} a ON n.vid = a.vid WHERE a.tag = '%s' AND n.status = 1 GROUP BY a.value, a.clean ORDER BY a.value ASC"), $tag);
      while ($obj = db_fetch_object($result)) {
        $items[] = l($obj->value, "audio/by/{$tag}/{$obj->clean}") ." ({$obj->count})";
      }
      $output = theme('item_list', $items);
    }
  }
  else {
    $items = array();
    $settings = audio_get_tag_settings();
    $result = db_query(db_rewrite_sql('SELECT DISTINCT a.tag FROM {node} n INNER JOIN {audio_metadata} a ON n.vid = a.vid WHERE n.status = 1 ORDER BY a.tag ASC'));
    while ($obj = db_fetch_object($result)) {
      if ($settings[$obj->tag]['browsable']) {
        $items[] = l($obj->tag, 'audio/by/'. $obj->tag);
      }
    }
    $output = theme('item_list', $items);
  }

  drupal_set_breadcrumb($breadcrumb);

  return $output;
}


/**
 * Fetches an audio file, allows "shorthand" urls such of the form:
 * audio/view/$nid (e.g. audio/download/25/ or audio/download/14)
 *
 * @param $nid
 *   Node ID.
 */
function audio_download($node) {
  // Increment the play count.
  db_query('UPDATE {audio} SET download_count = download_count + 1 WHERE vid = %d', $node->vid);

  // Downloading counts as "viewing" the node.
  node_tag_new($node->nid);

  // Notify other modules.
  audio_invoke_audioapi('download', $node);

  // The mime_header_encode function does not (yet) support
  // quoted-string encoding of ASCII strings with special
  // characters.  See discussion at http://drupal.org/node/82614
  $filename = $node->audio['file']->filename;
  // If the string contains non-ASCII characters, process it through
  // the mime_header_encode function.
  if (preg_match('/[^\x20-\x7E]/', $filename)) {
    $filename = mime_header_encode($filename);
  }
  // Otherwise, if the string contains special characters (like
  // space), perform quoted-string encoding.
  elseif (preg_match('/[ \(\)<>@,;:\\"\/\[\]\?=]/', $filename)) {
    $filename = '"'. str_replace('"', '\"', $filename) .'"';
  }
  $headers = array(
    'Content-Type: '. mime_header_encode($node->audio['file']->filemime),
    'Content-Length: '. $node->audio['file']->filesize,
    'Content-Disposition: attachment; filename='. $filename,
  );
  audio_file_transfer($node->audio['file']->filepath, $headers);
}

/**
 * Similar to audio_download, but for streaming playback instead of download.
 *
 * @param $nid
 *   Node ID.
 */
function audio_play($node = FALSE) {
  // Increment the play count.
  db_query('UPDATE {audio} SET play_count = play_count + 1 WHERE vid = %d', $node->vid);

  // Playing counts as "viewing" the node.
  node_tag_new($node->nid);

  // Notify other modules.
  audio_invoke_audioapi('play', $node);

  $headers = array(
    'Pragma: public',
    'Expires: 0',
    'Cache-Control: must-revalidate, post-check=0, pre-check=0, private',
    'Content-Type: '. mime_header_encode($node->audio['file']->filemime),
    'Content-Length: '. $node->audio['file']->filesize,
    'Content-Disposition: inline;',
    'Content-Transfer-Encoding: binary',
  );
  // Required for IE, otherwise Content-disposition is ignored.
  ini_set('zlib.output_compression', 'Off');
  audio_file_transfer($node->audio['file']->filepath, $headers);
}

/**
 * Transfer a file to the client.
 *
 * Variation on Drupal's file_transfer() function. The only difference
 * is that set_time_limit() is called to allow for large files.
 *
 * @param $source
 *   String specifying the file to transfer.
 * @param $headers
 *   An array of http headers to send along with file.
 */
function audio_file_transfer($source, $headers) {
  @ob_end_clean();

  foreach ($headers as $header) {
    // To prevent HTTP header injection, we delete new lines that are
    // not followed by a space or a tab.
    // See http://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html#sec4.2
    $header = preg_replace('/\r?\n(?!\t| )/', '', $header);
    header($header);
  }

  $source = file_create_path($source);

  // Transfer file in 1024 byte chunks to save memory usage.
  if ($fd = fopen($source, 'rb')) {
    if (!ini_get('safe_mode')) {
      set_time_limit(0);
    }
    while (!feof($fd)) {
      print fread($fd, 1024);
    }
    fclose($fd);
  }
  else {
    drupal_not_found();
  }
  exit();
}
