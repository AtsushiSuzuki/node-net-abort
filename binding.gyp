{
  "targets": [
    {
      "target_name": "net_abort",
      "sources": ["src/net_abort.cc", "lib/net-abort.js", "test/test.net-abort.js"],
      "conditions": [
        [ "OS==\"win\"", {
          "libraries": ["-lws2_32"]
        }]
      ]
    }
  ]
}