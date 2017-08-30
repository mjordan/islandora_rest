<?php

/**
 * @file
 * Generate responses for the object 'export' rest end point.
 */

/**
 * Responds to the GET 'export' request for this resource.
 *
 * This function assumes the requested resource is the subject of all returned
 * triples.
 *
 * @param array $parameters
 *   An associative array containing relevent data for this request. See
 *   https://wiki.duraspace.org/display/FEDORA38/REST+API#RESTAPI-export
 *   for more detail.
 *   - resource: The object to generate the response for.
 *   - request: The request parameters
 *     - format: The format of the exported XML. Must be one of the keys
 *       in $formats, below (required).
 *     - context: The context of the exported XML. Must be one of 'public',
 *       'migrate', or 'archive' (required).
 *     - encoding: The encoding of the exported XML. Defaults to UTF-8
 *       (optional).
 */
function islandora_rest_export_get_response(array $parameters) {
  $object = $parameters['resource'];
  $request = $parameters['request'];
  $format = $request['format'];
  $context = $request['context'];
  $encoding = isset($request['encoding']) ? $request['encoding'] : 'UTF-8';

  // Shorter versions of the 'format' URIs.
  $formats = array(
    'FOXML-1.1' => 'info:fedora/fedora-system:FOXML-1.1',
    'FOXML-1.0' => 'info:fedora/fedora-system:FOXML-1.0',
    'METSFedoraExt-1.1' => 'info:fedora/fedora-system:METSFedoraExt-1.1',
    'METSFedoraExt-1.0' => 'info:fedora/fedora-system:METSFedoraExt-1.0',
    'ATOM-1.1' => 'info:fedora/fedora-system:ATOM-1.1',
    'ATOMZip-1.1' => 'info:fedora/fedora-system:ATOMZip-1.1',
  );

  // Use the Tuque API-M to export the XML for the object.
  global $user;
  $fedora_url = variable_get('islandora_base_url', 'http://localhost:8080/fedora');
  $connection = islandora_get_tuque_connection($user, $fedora_url);
  $foxml = $connection->api->m->export($object->id,
    array(
      'format' => $formats[$format],
      'context' => $context,
      'encoding' => $encoding,
    )
  );
  islandora_rest_xml_download($foxml, $encoding);
}

/**
 * Download the output as XML.
 *
 * @param string $output
 *   The content to download.
 */
function islandora_rest_xml_download($output, $encoding = 'utf8') {
  drupal_add_http_header('Content-type', 'text/xml;charset=' . $encoding);
  drupal_add_http_header('Content-length', strlen($output));
  drupal_add_http_header('Content-Disposition', 'attachment; filename="export.xml"');
  print $output;
  drupal_page_footer();
}
