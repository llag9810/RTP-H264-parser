cmake_minimum_required(VERSION 3.13)
project(rtp_parser)

set(CMAKE_CXX_STANDARD 14)

add_executable(rtp_parser main.cpp PcapManager.cpp PcapManager.h NetworkProtocolParser.cpp NetworkProtocolParser.h H264parser.cpp H264parser.h)
target_link_libraries(rtp_parser pcap)