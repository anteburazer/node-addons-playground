{
  "targets": [
        {
            "target_name": "sensor_simulator",
            "sources": ["./cpp/sensor_simulator.cpp"],
            "cflags": ["-Wall", "-std=c++11"],
            "cflags!": ['-fno-exceptions' ],
            "cflags_cc!": ['-fno-exceptions'],
            "include_dirs" : ["<!(node -e \"require('nan')\")", "../common"],
            'xcode_settings': {
                'OTHER_CFLAGS': ['-std=c++11'],
            },
            "conditions": [
                ['OS=="mac"', {
                    "xcode_settings": {
                        'OTHER_CPLUSPLUSFLAGS' : ['-std=c++11','-stdlib=libc++'],
                        'OTHER_LDFLAGS': ['-stdlib=libc++'],
                        'MACOSX_DEPLOYMENT_TARGET': '10.7' , 
                        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'}
                    }
                ]
            ]
        }
    ]
}