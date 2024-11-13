#pragma once

#include "ZoneMasterDataPubSubTypes.h"
//#include "ZoneMasterLinMessageDataListener.h"

#include <chrono>
#include <thread>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

using namespace eprosima::fastdds::dds;


class LinParserListener : public DataReaderListener
{
public:
    LinParserListener()
        : samples_(0)
    {
    }

    ~LinParserListener() override
    {
    }

    void on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info) override
    {
        if (info.current_count_change == 1)
            {
                std::cout << "LinParserSubscriber matched.";
            }
        else if (info.current_count_change == -1)
            {
                std::cout << "LinParserSubscriber unmatched.";
            }
        else
            {
                std::cout << info.current_count_change;
            }
    }

    void on_data_available(DataReader* reader) override
    {
        SampleInfo info;
        auto retcode = reader->take_next_sample(&linframes_, &info);
        if (retcode == ReturnCode_t::RETCODE_OK)
            {
                if (info.valid_data)
                    {
                        samples_++;
                    }
            }
        else if (retcode == ReturnCode_t::RETCODE_NO_DATA) {
            std::cout << "No data available, retrying...";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void on_requested_incompatible_qos(
                                       DataReader* reader,
                                       const RequestedIncompatibleQosStatus& status)
    {
        (void)reader;
        (void)status;
    }

    linFrames linframes_;
    std::atomic_int samples_;
};
