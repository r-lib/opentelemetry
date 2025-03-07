#' Tracer provider to export over HTTP
#' @export

tracer_provider_http <- list(
  new = function() {
    self <- new_object(
      c("opentelemetry_tracer_provider_http",
        "opentelemetry_tracer_provider"),
      get_tracer = function(name, ...) {
        tracer$new(self, name, ...)
      }
    )

    # TODO
#    self$xptr <- .Call(otel_create_tracer_provider_http)
    self
  }
)
