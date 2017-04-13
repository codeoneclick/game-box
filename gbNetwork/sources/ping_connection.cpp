//
//  ping_connection.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 4/11/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ping_connection.h"

namespace gb
{
    namespace net
    {
        class icmp_header
        {
        public:
            enum
            {
                echo_reply = 0, destination_unreachable = 3, source_quench = 4,
                redirect = 5, echo_request = 8, time_exceeded = 11, parameter_problem = 12,
                timestamp_request = 13, timestamp_reply = 14, info_request = 15,
                info_reply = 16, address_request = 17, address_reply = 18
            };
            
            icmp_header()
            {
                std::fill(rep_, rep_ + sizeof(rep_), 0);
            }
            
            ui8 type() const
            {
                return rep_[0];
            }
            
            ui8 code() const
            {
                return rep_[1];
            }
            
            ui16 checksum() const
            {
                return decode(2, 3);
            }
            
            ui16 identifier() const
            {
                return decode(4, 5);
            }
            
            ui16 sequence_number() const
            {
                return decode(6, 7);
            }
            
            void type(ui8 n)
            {
                rep_[0] = n;
            }
            
            void code(ui8 n)
            {
                rep_[1] = n;
            }
            
            void checksum(ui16 n)
            {
                encode(2, 3, n);
            }
            
            void identifier(ui16 n)
            {
                encode(4, 5, n);
            }
            
            void sequence_number(ui16 n)
            {
                encode(6, 7, n);
            }
            
            friend std::istream& operator>>(std::istream& is, icmp_header& header)
            {
                return is.read(reinterpret_cast<char*>(header.rep_), 8);
            }
            
            friend std::ostream& operator<<(std::ostream& os, const icmp_header& header)
            {
                return os.write(reinterpret_cast<const char*>(header.rep_), 8);
            }
            
        private:
            
            ui16 decode(i32 a, i32 b) const
            {
                return (rep_[a] << 8) + rep_[b];
            }
            
            void encode(i32 a, i32 b, ui16 n)
            {
                rep_[a] = static_cast<ui8>(n >> 8);
                rep_[b] = static_cast<ui8>(n & 0xFF);
            }
            
            ui8 rep_[8];
        };
        
        template <typename Iterator>
        void compute_checksum(icmp_header& header,
                              Iterator body_begin, Iterator body_end)
        {
            ui32 sum = (header.type() << 8) + header.code()
            + header.identifier() + header.sequence_number();
            
            Iterator body_iter = body_begin;
            while (body_iter != body_end)
            {
                sum += (static_cast<ui8>(*body_iter++) << 8);
                if (body_iter != body_end)
                {
                    sum += static_cast<ui8>(*body_iter++);
                }
            }
            
            sum = (sum >> 16) + (sum & 0xFFFF);
            sum += (sum >> 16);
            header.checksum(static_cast<ui16>(~sum));
        }
        
        class ipv4_header
        {
        public:
            
            ipv4_header() { std::fill(rep_, rep_ + sizeof(rep_), 0); }
            
            ui8 version() const
            {
                return (rep_[0] >> 4) & 0xF;
            }
            
            ui16 header_length() const
            {
                return (rep_[0] & 0xF) * 4;
            }
            
            ui8 type_of_service() const
            {
                return rep_[1];
            }
            
            ui16 total_length() const
            {
                return decode(2, 3);
            }
            
            ui16 identification() const
            {
                return decode(4, 5);
            }
            
            bool dont_fragment() const
            {
                return (rep_[6] & 0x40) != 0;
            }
            
            bool more_fragments() const
            {
                return (rep_[6] & 0x20) != 0;
            }
            
            ui16 fragment_offset() const
            {
                return decode(6, 7) & 0x1FFF;
            }
            
            ui32 time_to_live() const
            {
                return rep_[8];
            }
            
            ui8 protocol() const
            {
                return rep_[9];
            }
            
            ui16 header_checksum() const
            {
                return decode(10, 11);
            }
            
            asio::ip::address_v4 source_address() const
            {
                asio::ip::address_v4::bytes_type bytes
                = { { rep_[12], rep_[13], rep_[14], rep_[15] } };
                return asio::ip::address_v4(bytes);
            }
            
            asio::ip::address_v4 destination_address() const
            {
                asio::ip::address_v4::bytes_type bytes
                = { { rep_[16], rep_[17], rep_[18], rep_[19] } };
                return asio::ip::address_v4(bytes);
            }
            
            friend std::istream& operator>>(std::istream& is, ipv4_header& header)
            {
                is.read(reinterpret_cast<char*>(header.rep_), 20);
                if (header.version() != 4)
                {
                    is.setstate(std::ios::failbit);
                }
                std::streamsize options_length = header.header_length() - 20;
                if (options_length < 0 || options_length > 40)
                {
                    is.setstate(std::ios::failbit);
                }
                else
                {
                    is.read(reinterpret_cast<char*>(header.rep_) + 20, options_length);
                }
                return is;
            }
            
        private:
            
            ui16 decode(i32 a, i32 b) const
            {
                return (rep_[a] << 8) + rep_[b];
            }
            
            ui8 rep_[60];
        };
        
        class ping_connection_pimpl
        {
        private:
            
            asio::io_service& m_io_service;
            asio::ip::icmp::resolver m_resolver;
            asio::ip::icmp::endpoint m_destination;
            asio::streambuf m_reply_buffer;
            
        protected:
            
        public:
            
            ping_connection_pimpl(asio::io_service& io_service) :
            m_io_service(io_service),
            m_resolver(io_service)
            {
                
            }
            
            ~ping_connection_pimpl()
            {
                
            }
            
            asio::ip::icmp::resolver& get_resolver()
            {
                return m_resolver;
            };
            
            asio::ip::icmp::endpoint& get_destination()
            {
                return m_destination;
            };
            
            asio::streambuf& get_reply_buffer()
            {
                return m_reply_buffer;
            };
            
            asio::io_service& get_io_service()
            {
                return m_io_service;
            };
        };

        
        ping_connection::ping_connection(asio::io_service& io_service) :
        m_socket(io_service, asio::ip::icmp::v4(), 6868),
        m_pimpl(std::make_shared<ping_connection_pimpl>(io_service)),
        m_ping_callback(nullptr),
        m_is_ended(false)
        {
            
        }
        
        ping_connection::~ping_connection()
        {
            m_thread_io.join();
        }
        
        void ping_connection::establish(const std::string& ip, i32 port)
        {
            std::stringstream port_str;
            port_str<<port;
            asio::ip::icmp::resolver resolver(m_pimpl->get_io_service());
            asio::ip::icmp::resolver::query query(ip, port_str.str());
            asio::ip::icmp::resolver::iterator endpoint_iterator = resolver.resolve(query);
            asio::ip::icmp::resolver::iterator end;
            
            if(endpoint_iterator != end)
            {
                m_pimpl->get_destination() = *(endpoint_iterator);
            }
            
            std::string body("game-box-ping");
            ui16 process_id = 0;
            
#if defined(__WIN32__)
            
            process_id = static_cast<ui16>(::GetCurrentProcessId());
#else
            
            process_id = static_cast<ui16>(::getpid());
            
#endif
            
            icmp_header echo_request;
            echo_request.type(icmp_header::echo_request);
            echo_request.code(0);
            echo_request.identifier(process_id);
            echo_request.sequence_number(1);
            compute_checksum(echo_request, body.begin(), body.end());
            
            asio::streambuf request_buffer;
            std::ostream os(&request_buffer);
            os<<echo_request<<body;
            
            std::error_code ec;
            m_socket.open(asio::ip::icmp::v4(), ec);
            std::cout<<"ping socket open with error code: "<<ec.value()<<", "<<ec.category().name()<<std::endl;
            std::size_t sent_size = m_socket.send_to(request_buffer.data(), m_pimpl->get_destination(), 0, ec);
            std::cout<<"ping sent: "<<sent_size<<" bytes with error code: "<<ec.value()<<", "<<ec.category().name()<<std::endl;
            
            ping_connection::start();
            
            m_pimpl->get_reply_buffer().consume(m_pimpl->get_reply_buffer().size());
            m_socket.async_receive(m_pimpl->get_reply_buffer().prepare(65536), std::bind(&ping_connection::receive, this, std::placeholders::_2));
        }
        
        void ping_connection::start()
        {
            m_thread_io = std::thread(&ping_connection::run, this);
        }
        
        void ping_connection::run()
        {
#if defined(__IOS__) || defined(__OSX__) || defined(__TVOS__)
            
            pthread_setname_np("gb.core.ping_connection.run");
            
#endif
            
            while(!m_is_ended)
            {
                std::error_code ec;
                asio::io_service::work work(m_socket.get_io_service());
                m_socket.get_io_service().run(ec);
                assert(!ec);
            }
        }
        
        void ping_connection::receive(std::size_t length)
        {
            m_pimpl->get_reply_buffer().commit(length);
            
            std::istream is(&m_pimpl->get_reply_buffer());
            ipv4_header ipv4_hdr;
            icmp_header icmp_hdr;
            is >> ipv4_hdr >> icmp_hdr;
            
            if (is && icmp_hdr.type() == icmp_header::echo_reply)
            {
                std::cout<<length - ipv4_hdr.header_length()
                <<" bytes from " << ipv4_hdr.source_address()
                <<": icmp_seq=" << icmp_hdr.sequence_number()
                <<", ttl=" << ipv4_hdr.time_to_live()
                <<std::endl;
            }
        }
        
        void ping_connection::timeout()
        {
            
        }
        
        void ping_connection::set_ping_callback(const ping_callback_t& callback)
        {
            m_ping_callback = callback;
        }
    }
}
