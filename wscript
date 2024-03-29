# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass

# def configure(conf):
#     conf.check_nonfatal(header_name='stdint.h', define_name='HAVE_STDINT_H')

def build(bld):
    module = bld.create_ns3_module('lifi', ['internet', 'mobility'])
    module.source = [
        'model/lifi-propagation-loss-model.cc',
        'model/lifi-snr.cc',
        'model/lifi-phy.cc',
        'model/lifi-mobility-model.cc', 
        'model/lifi-error-model.cc',
        'model/lifi-channel.cc',
        'model/lifi-net-device.cc',
        'model/lifi-tx-net-device.cc',
        'model/lifi-rx-net-device.cc',
        'model/lifi-mac-low.cc',
        'model/lifi-ap-mac.cc',
        'model/lifi-sta-mac.cc',
        'model/lifi-tx-mac.cc',
        'model/lifi-rx-mac.cc',
        'model/lifi-mac-queue.cc',
        'model/lifi-user-device-infos.cc',
        'model/beacon-response-tag.cc',
        'model/lifi-application.cc',
        'model/lifi-regular-mac.cc',
        'model/PerformanceAnalysis.cc',
        'model/lifi-capability-information.cc',
        'model/mac-resources-tag.cc',
        'model/remote-station-manager.cc',
        'helper/lifi-helper.cc', 
        'helper/wifi-ap-helper.cc',
        'helper/lifi-phy-helper.cc',
        'helper/lifi-mac-helper.cc',
        'model/wifi-ap-net-device.cc',
        'model/lifi-random-waypoint-mobility-model.cc',
        'model/lifi-constant-velocity-mobility-model.cc',
        'model/received-data-model.cc',
        'model/my-tag.cc',
        ]

    module_test = bld.create_ns3_module_test_library('simple_lifi')
    module_test.source = [
        #'test/lifi-test-suite.cc',
        ]

    headers = bld(features=['ns3header'])
    headers.module = 'lifi'
    headers.source = [
        'model/lifi-phy.h',
        'model/lifi-snr.h',
        'model/lifi-propagation-loss-model.h',
        'model/lifi-mobility-model.h', 
        'model/lifi-error-model.h',
        'model/lifi-channel.h',
        'model/lifi-ap-mac.h',
        'model/lifi-sta-mac.h',
        'model/lifi-tx-mac.h',
        'model/lifi-rx-mac.h',
        'model/lifi-net-device.h',
        'model/lifi-tx-net-device.h',
        'model/lifi-rx-net-device.h',
        'model/lifi-mac-low.h',
        'model/lifi-mac-queue.h',
        'model/lifi-user-device-infos.h',
        'model/lifi-regular-mac.h',
        'model/lifi-application.h', 
        'model/PerformanceAnalysis.h',
        'model/lifi-capability-information.h',
        'model/mac-resources-tag.h',
        'model/beacon-response-tag.h',
        'helper/lifi-helper.h', 
        'helper/wifi-ap-helper.h',
        'helper/lifi-phy-helper.h',
        'helper/lifi-mac-helper.h',
        'model/wifi-ap-net-device.h',
        'model/remote-station-manager.h',
        'model/commons.h',
        'model/lifi-random-waypoint-mobility-model.h',
        'model/lifi-constant-velocity-mobility-model.h',
        'model/received-data-model.h',
        'model/my-tag.h',
        ]

   # module.env.append_value("CXXFLAGS", "-I/usr/include")
    #module.env.append_value("LINKFLAGS", ["-L/usr/lib/python2.7/dist-packages"])
    #module.env.append_value("LIBPATH", ["/usr/lib/x86_64-linux-gnu"])

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')

    # bld.ns3_python_bindings()

