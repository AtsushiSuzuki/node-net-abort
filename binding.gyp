{
  "targets": [
    {
      "target_name": "net_abort",
      "include_dirs": [
      	"<(node_root_dir)/deps/cares/include",
      	"<(node_root_dir)/deps/debugger-agent/include"
      ],
      "sources": ["src/net_abort.cc", "lib/net-abort.js", "test/test.net-abort.js"],
      "conditions": [
        [ "OS==\"win\"", {
          "libraries": ["-lws2_32"]
        }]
      ]
    }
  ]
}