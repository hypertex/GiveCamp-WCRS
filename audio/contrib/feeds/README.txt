ABOUT
This module provides XSPF, M3U, and PLS audio feeds for audio files attached to
a node (audio_attach.module) or views generated list of audio content (views.module).

INSTALLATION
Turn on the module at (admin/build/modules)
Configure this module for usage with views or audio attachments.  See below.


USAGE WITH VIEWS.MODULE
Go to the views administration menu and you should see a new view type for audio feeds.
Configure your view to output a list of audio nodes.

your/view/path:         - returns a themed view with xspf player and permalinks to feeds.
your/view/path?xspf     - returns xspf XML
your/view/path?m3u      - returns m3u XML
your/view/path?pls      - returns pls XML


USAGE WITH AUDIO_ATTACH.MODULE
Configure which node types can generate feeds. (admin/content/types)
Any node that you've attached a list of audio files to will automatically generate these three feeds (XSPF, M3U, PLS).

For instance, if node/123 was a node with a list of audio files attached, the feed urls would be:
node/123/xspf           - returns xspf XML
node/123/m3u            - returns m3u XML
node/123/pls            - returns pls XML


TODO
bring back podcast feeds or integrate with audio itunes module
block view, resizable player



