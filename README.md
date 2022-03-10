# minijson

minimal JSON parser

## Usage

```cpp
const char* foo = R"(
{"empty":{"":[]}, "foo":
3.2, "bar": "baz", "baz": true,"hoge":{"a":"あい\nうお\2え"}, "moge": null, "zz": 0xff
, "rrr":[[2,"3"], "4"], "fuba": 1e9}
)";
minijson::error e = minijson::parse(p, v);
```

The pointer p will be shift to the end of token. So it is the caller's responsibility to check whether the extra tokens are treated as errors.

## License

MIT

## Author

Yasuhiro Matsumoto
