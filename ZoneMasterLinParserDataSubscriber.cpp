#include "ZoneMasterLinParserDataListener.h"
#include "ZoneMasterLinParserDataSubscriber.h"

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.h>

bool ZoneMasterLinParserSubscriber::init()
{
    DomainParticipantQos participantQos;
    participantQos.name("P_Zweig");
    //std::shared_ptr<eprosima::fastdds::rtps::SharedMemTransportDescriptor> shm_transport = std::make_shared<eprosima::fastdds::rtps::SharedMemTransportDescriptor>();
    //participantQos.transport().user_transports.push_back(shm_transport);
    participant_ = DomainParticipantFactory::get_instance()->create_participant(domainid, participantQos);
    if (participant_ == nullptr)
    {
        return false;
    }

    // Register the Type
    participant_->register_type(type_);

    TopicQos tqos;
    participant_->get_default_topic_qos(tqos);

    // Create the subscriptions Topic
    topic_ = participant_->create_topic(
        "linParserTopic",
        //type_.get_type_name(),
        "linParserData",
        tqos);
    if (topic_ == nullptr)
    {
        std::cout << "Topic Null" << std::endl;
        return false;
    }

    // Create the Subscriber
    SubscriberQos subscriber_qos = SUBSCRIBER_QOS_DEFAULT;
    participant_->get_default_subscriber_qos(subscriber_qos);
    subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);
    if (subscriber_ == nullptr)
    {
        return false;
    }

    // Create the DataReader
    DataReaderQos reader_qos = DATAREADER_QOS_DEFAULT;
    reader_qos.reliability().kind = eprosima::fastdds::dds::BEST_EFFORT_RELIABILITY_QOS;
    reader_qos.durability().kind = eprosima::fastdds::dds::VOLATILE_DURABILITY_QOS;
    reader_qos.data_sharing().automatic();
    subscriber_->set_default_datareader_qos(reader_qos);

    reader_ = subscriber_->create_datareader(topic_, reader_qos, &listener_);

    if (reader_ == nullptr)
        {
            return false;
        }
    std::cout << "READER CREATED SUCCESSFULLY." << std::endl;
    return true;

}

void ZoneMasterLinParserSubscriber::run()
{
}

void ZoneMasterLinParserSubscriber::run(uint32_t number)
{
    std::cout << "SUBSCRIBER RUNNING UNTIL " << number << "." << std::endl;
    while (number > listener_.samples_)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
