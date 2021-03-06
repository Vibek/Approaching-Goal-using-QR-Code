/* Auto-generated by genmsg_cpp for file /home/vibek/QRcode_detection/msg/UIConfirm.msg */
#ifndef QRCODE_DETECTION_MESSAGE_UICONFIRM_H
#define QRCODE_DETECTION_MESSAGE_UICONFIRM_H
#include <string>
#include <vector>
#include <map>
#include <ostream>
#include "ros/serialization.h"
#include "ros/builtin_message_traits.h"
#include "ros/message_operations.h"
#include "ros/time.h"

#include "ros/macros.h"

#include "ros/assert.h"


namespace QRcode_detection
{
template <class ContainerAllocator>
struct UIConfirm_ {
  typedef UIConfirm_<ContainerAllocator> Type;

  UIConfirm_()
  : id(0)
  , confirmed(false)
  {
  }

  UIConfirm_(const ContainerAllocator& _alloc)
  : id(0)
  , confirmed(false)
  {
  }

  typedef int32_t _id_type;
  int32_t id;

  typedef uint8_t _confirmed_type;
  uint8_t confirmed;


  typedef boost::shared_ptr< ::QRcode_detection::UIConfirm_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::QRcode_detection::UIConfirm_<ContainerAllocator>  const> ConstPtr;
  boost::shared_ptr<std::map<std::string, std::string> > __connection_header;
}; // struct UIConfirm
typedef  ::QRcode_detection::UIConfirm_<std::allocator<void> > UIConfirm;

typedef boost::shared_ptr< ::QRcode_detection::UIConfirm> UIConfirmPtr;
typedef boost::shared_ptr< ::QRcode_detection::UIConfirm const> UIConfirmConstPtr;


template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const  ::QRcode_detection::UIConfirm_<ContainerAllocator> & v)
{
  ros::message_operations::Printer< ::QRcode_detection::UIConfirm_<ContainerAllocator> >::stream(s, "", v);
  return s;}

} // namespace QRcode_detection

namespace ros
{
namespace message_traits
{
template<class ContainerAllocator> struct IsMessage< ::QRcode_detection::UIConfirm_<ContainerAllocator> > : public TrueType {};
template<class ContainerAllocator> struct IsMessage< ::QRcode_detection::UIConfirm_<ContainerAllocator>  const> : public TrueType {};
template<class ContainerAllocator>
struct MD5Sum< ::QRcode_detection::UIConfirm_<ContainerAllocator> > {
  static const char* value() 
  {
    return "86a9b286e1d2e325349676b33c0197a6";
  }

  static const char* value(const  ::QRcode_detection::UIConfirm_<ContainerAllocator> &) { return value(); } 
  static const uint64_t static_value1 = 0x86a9b286e1d2e325ULL;
  static const uint64_t static_value2 = 0x349676b33c0197a6ULL;
};

template<class ContainerAllocator>
struct DataType< ::QRcode_detection::UIConfirm_<ContainerAllocator> > {
  static const char* value() 
  {
    return "QRcode_detection/UIConfirm";
  }

  static const char* value(const  ::QRcode_detection::UIConfirm_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct Definition< ::QRcode_detection::UIConfirm_<ContainerAllocator> > {
  static const char* value() 
  {
    return "int32 id\n\
bool confirmed\n\
\n\
";
  }

  static const char* value(const  ::QRcode_detection::UIConfirm_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator> struct IsFixedSize< ::QRcode_detection::UIConfirm_<ContainerAllocator> > : public TrueType {};
} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

template<class ContainerAllocator> struct Serializer< ::QRcode_detection::UIConfirm_<ContainerAllocator> >
{
  template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
  {
    stream.next(m.id);
    stream.next(m.confirmed);
  }

  ROS_DECLARE_ALLINONE_SERIALIZER;
}; // struct UIConfirm_
} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::QRcode_detection::UIConfirm_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const  ::QRcode_detection::UIConfirm_<ContainerAllocator> & v) 
  {
    s << indent << "id: ";
    Printer<int32_t>::stream(s, indent + "  ", v.id);
    s << indent << "confirmed: ";
    Printer<uint8_t>::stream(s, indent + "  ", v.confirmed);
  }
};


} // namespace message_operations
} // namespace ros

#endif // QRCODE_DETECTION_MESSAGE_UICONFIRM_H

