// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file HelloWorldSubscriber.cpp
 *
 */

#include "HelloWorldSubscriber.h"
#include "ZoneMasterLinParserDataSubscriber.h"

#include <chrono>
#include <thread>

#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>

using namespace eprosima::fastdds::dds;

HelloWorldSubscriber::HelloWorldSubscriber()
    : participant_(nullptr)
    , subscriber_(nullptr)
    , topic_(nullptr)
    , reader_(nullptr)
      //, type_(new HelloWorldPubSubType())
    , type_(nullptr)
{
    linFramesPubSubType* lin_frame_pubsubtype = new linFramesPubSubType();
    lin_frame_pubsubtype->setName("linParserData");
    type_ = TypeSupport(lin_frame_pubsubtype);
}

bool HelloWorldSubscriber::init(
        bool use_env)
{
    DomainParticipantQos pqos = PARTICIPANT_QOS_DEFAULT;
    pqos.name("Participant_sub");
    auto factory = DomainParticipantFactory::get_instance();

    // if (use_env)
    //     {
    //         factory->load_profiles();
    //         factory->get_default_participant_qos(pqos);
    //     }

    //participant_ = factory->create_participant(0, pqos);
    participant_ = factory->create_participant(83, pqos);

    if (participant_ == nullptr)
        {
            return false;
        }

    //REGISTER THE TYPE
    //type_.register_type(participant_);
    participant_->register_type(type_);

    //CREATE THE SUBSCRIBER
    SubscriberQos sqos = SUBSCRIBER_QOS_DEFAULT;

    if (use_env)
        {
            participant_->get_default_subscriber_qos(sqos);
        }

    subscriber_ = participant_->create_subscriber(sqos, nullptr);

    if (subscriber_ == nullptr)
        {
            return false;
        }
    std::cout << "SUBCRIBER CREATED." << std::endl;

    //CREATE THE TOPIC
    TopicQos tqos = TOPIC_QOS_DEFAULT;
    participant_->get_default_topic_qos(tqos);

    if (use_env)
        {
            participant_->get_default_topic_qos(tqos);
        }

    topic_ = participant_->create_topic(
                                        //"HelloWorldTopic",
                                        //"HelloWorld",
                                        "linParserTopic",
                                        type_.get_type_name(),
                                        tqos);

    if (topic_ == nullptr)
        {
            std::cout << "TOPIC NOT CREATED." << std::endl;
            return false;
        }
    std::cout << "TOPIC CREATED." << std::endl;

    // CREATE THE READER
    DataReaderQos rqos = DATAREADER_QOS_DEFAULT;
    //rqos.reliability().kind = RELIABLE_RELIABILITY_QOS;
    rqos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;
    rqos.durability().kind = VOLATILE_DURABILITY_QOS;
    rqos.data_sharing().automatic();

    if (use_env)
        {
            subscriber_->get_default_datareader_qos(rqos);
        }

    reader_ = subscriber_->create_datareader(topic_, rqos, &listener_);

    if (reader_ == nullptr)
        {
            std::cout << "READER NOT CREATED." << std::endl;
            return false;
        }
    std::cout << "READER CREATED." << std::endl;
    return true;
}

HelloWorldSubscriber::~HelloWorldSubscriber()
{
    if (reader_ != nullptr)
    {
        subscriber_->delete_datareader(reader_);
    }
    if (topic_ != nullptr)
    {
        participant_->delete_topic(topic_);
    }
    if (subscriber_ != nullptr)
    {
        participant_->delete_subscriber(subscriber_);
    }
    DomainParticipantFactory::get_instance()->delete_participant(participant_);
}

void HelloWorldSubscriber::SubListener::on_subscription_matched(
        DataReader*,
        const SubscriptionMatchedStatus& info)
{
    std::cout << "ON SUBSCRIPTION MATCHED" << std::endl;
    if (info.current_count_change == 1)
    {
        matched_ = info.total_count;
        std::cout << "Subscriber matched." << std::endl;
    }
    else if (info.current_count_change == -1)
    {
        matched_ = info.total_count;
        std::cout << "Subscriber unmatched." << std::endl;
    }
    else
    {
        std::cout << info.current_count_change
                  << " is not a valid value for SubscriptionMatchedStatus current count change" << std::endl;
    }
}

void HelloWorldSubscriber::SubListener::on_data_available(
                                                          DataReader* reader)
{
    std::cout << "ON DATA AVAILABLE" << std::endl;
    SampleInfo info;
    if (reader->take_next_sample(&hello_, &info) == ReturnCode_t::RETCODE_OK)
        {
            if (info.instance_state == ALIVE_INSTANCE_STATE)
                {
                    samples_++;
                    // Print your structure data here.
                    //std::cout << "Message " << hello_.message() << " " << hello_.index() << " RECEIVED" << std::endl;
                    std::cout << "Message" << " RECEIVED" << std::endl;
                }
        }
}

void HelloWorldSubscriber::run()
{
    std::cout << "Subscriber running. Please press enter to stop the Subscriber" << std::endl;
    std::cin.ignore();
}

void HelloWorldSubscriber::run(
        uint32_t number)
{
    std::cout << "Subscriber running until " << number << "samples have been received" << std::endl;
    while (number > listener_.samples_)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
