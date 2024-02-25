# SML2HTTP

An ESP8266 based smart meter (SML) to HTTP/JSON gateway

This project is a fork of the [SML to MQTT gateway](https://github.com/mruettgers/SMLReader).

## About

The aim of this project is to read the meter readings of modern energy meters and make them available via HTTP/JSON.

The software was primarily developed and tested for the EMH ED300L electricity meter, but should also work with other energy meters that have an optical interface and communicate via the SML protocol.


## Make LSP work

```sh
pio run -t compiledb
```


## Install software on ESP8266

```sh
pio run -e d1_mini -t upload
```


## License

Distributed under the GPL v3 license.
See [LICENSE](LICENSE) for more information.
