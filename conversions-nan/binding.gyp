{
    "targets": [
        {
            "target_name": "basic_nan",
            "sources": [ "./cpp/basic_nan.cpp" ],
            "include_dirs" : [
                "<!(node -e \"require('nan')\")"
            ]
        }
    ]
}