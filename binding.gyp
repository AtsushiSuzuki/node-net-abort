{
  "targets": [
    {
      "target_name": "net_abort",
      "sources": [ "src/net_abort.cc" ],
      "conditions": [
        [ "OS==\"win\"", {
          "libraries": [ "-lws2_32" ]
        }]
      ]
    }
  ]
}