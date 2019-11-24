{
    "targets": [
        {
            "target_name": "factorization",
            "sources": ["./cpp/factorization_obj_wrap.cpp"],
            "cflags": ["-Wall", "-std=c++11"],
            "include_dirs": ["<!(node -e \"require('nan')\")", "../common"],
            'xcode_settings': {
                'OTHER_CFLAGS': ['-std=c++11'],
            },
            "conditions": [
                [ 'OS=="mac"', {
                    "xcode_settings": {
                        'OTHER_CPLUSPLUSFLAGS' : ['-std=c++11','-stdlib=libc++'],
                        'OTHER_LDFLAGS': ['-stdlib=libc++'],
                        'MACOSX_DEPLOYMENT_TARGET': '10.7' 
                    }
                }]
            ]
        }
    ]
}