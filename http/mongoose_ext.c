/*  SHTTPD Extensions
 *
 *  $Id: mongoose_ext.c,v 1.1 2009/11/23 14:31:53 joel Exp $
 */


#if defined(USE_MONGOOSE_HTTPD)

#include <rtems.h>
#include <stdio.h>
#include <mghttpd/mongoose.h>
#include <rtems/cpuuse.h>
#include <rtems/stackchk.h>

#include <stdio.h>

#define START_HTML_BODY \
      "HTTP/1.1 200 OK\r\n" \
      "Content-Type: text/html\r\n\r\n" \
      "<html><body>\r\n"

#define END_HTML_BODY \
      "</html></body>\r\n"

void example_mongoose_callback(
  struct mg_connection         *conn,
  const struct mg_request_info *request_info,
  void                         *user_data
)
{
  const char *query;

  query = mg_get_var(conn, "action" );
  if ( !query )
    query = "";
  /* fprintf( stderr, "RTEMS Request -%s-\n", query ); */

  mg_printf( conn, START_HTML_BODY "<pre>" );
  if ( !strcmp( query, "cpuuse_report" ) ) {
    rtems_cpu_usage_report_with_plugin(
      conn,
      (rtems_printk_plugin_t) mg_printf
    );
  } else if ( !strcmp( query, "cpuuse_reset" ) ) {
    rtems_cpu_usage_reset();
    mg_printf(
      conn,
      " <p><big>CPU Usage data reset -- return to the previous page</big></p>"
    );
  } else if ( !strcmp( query, "stackuse_report" ) ) {
    rtems_stack_checker_report_usage_with_plugin(
      conn,
      (rtems_printk_plugin_t) mg_printf
    );
  } else {
    mg_printf(
      conn,
      START_HTML_BODY
      " <h2>Unknown Request</h2>"
      " <h3>URI: %s</br>"
      "  Arguments: %s</h3>",
      mg_get_var(conn, "REQUEST_URI"),
      query
    );
  }
  mg_printf( conn, "</pre>" END_HTML_BODY );
  /*arg->flags |= SHTTPD_END_OF_OUTPUT; */
}

void example_mongoose_addpages(
  struct mg_context *server
)
{
  mg_set_uri_callback( server, "/queries*", example_mongoose_callback, NULL );
}

#endif
