/*  SHTTPD Extensions
 *
 *  $Id: shttpd_ext.c,v 1.4 2009/11/23 14:31:53 joel Exp $
 */


#if defined(USE_SIMPLE_HTTPD)

#include <rtems.h>
#include <shttpd/shttpd.h>
#include <rtems/cpuuse.h>
#include <rtems/stackchk.h>

#include <stdio.h>

#define START_HTML_BODY \
      "HTTP/1.1 200 OK\r\n" \
      "Content-Type: text/html\r\n\r\n" \
      "<html><body>\r\n"

#define END_HTML_BODY \

void example_shttpd_callback(struct shttpd_arg *arg)
{
  const char *query;

  query = shttpd_get_env(arg, "QUERY_STRING" );
  if ( !query )
    query = "";
  /* fprintf( stderr, "RTEMS Request -%s-\n", query ); */

  if ( !strcmp( query, "cpuuse_report" ) ) {

    shttpd_printf( arg, "<pre>" );
    rtems_cpu_usage_report_with_plugin(
      arg,
      (rtems_printk_plugin_t) shttpd_printf
    );
    shttpd_printf( arg, "</pre>" );
  } else if ( !strcmp( query, "cpuuse_reset" ) ) {
    rtems_cpu_usage_reset();
    shttpd_printf(
      arg,
      START_HTML_BODY
      " <p><big>CPU Usage data reset -- return to the previous page</big></p>"
      END_HTML_BODY
    );
  } else if ( !strcmp( query, "stackuse_report" ) ) {
    shttpd_printf( arg, "<pre>" );
    rtems_stack_checker_report_usage_with_plugin(
      arg,
      (rtems_printk_plugin_t) shttpd_printf
    );
    shttpd_printf( arg, "</pre>" );
  } else {
    shttpd_printf(
      arg,
      START_HTML_BODY
      " <h2>Unknown Request</h2>"
      " <h3>URI: %s</br>"
      "  Arguments: %s</h3>"
      END_HTML_BODY,
      shttpd_get_env(arg, "REQUEST_URI"),
      query
    );
  }
  arg->flags |= SHTTPD_END_OF_OUTPUT;
}

void example_shttpd_addpages(struct shttpd_ctx *ctx)
{
  shttpd_register_uri( ctx, "/queries*", example_shttpd_callback, NULL );
}

#endif
