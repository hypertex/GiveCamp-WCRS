// $Id: audio.js,v 1.2 2008/11/26 01:17:58 drewish Exp $

/**
 * Auto-attach standard client side file input validation.
 */
Drupal.behaviors.audioValidateAutoAttach = function(context) {
  $("input[@type='file'][audio_accept]", context).change( function() {
    $('.audio-js-error').remove();

    /**
     * Add client side validation for extensions.
     */
    var accept = $(this).attr('audio_accept');
    if (accept.length > 1) {
      v = new RegExp('\\.(' + accept.replace(/,\s*/g, '|') + ')$', 'gi');
      if (!v.test(this.value)) {
        var error = Drupal.t("The selected file %filename cannot not be uploaded. Only files with the following extensions are allowed: %extensions.",
          { '%filename' : this.value, '%extensions': accept }
        );
        // What do I prepend this to?
        $(this).before('<div class="messages error audio-js-error">' + error + '</div>');
        this.value = '';
        return false;
      }
    }
  });
}
