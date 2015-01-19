; Auto-generated. Do not edit!


(cl:in-package QRcode_detection-msg)


;//! \htmlinclude UIMessage.msg.html

(cl:defclass <UIMessage> (roslisp-msg-protocol:ros-message)
  ((id
    :reader id
    :initarg :id
    :type cl:integer
    :initform 0)
   (timeout
    :reader timeout
    :initarg :timeout
    :type cl:integer
    :initform 0)
   (msg
    :reader msg
    :initarg :msg
    :type cl:string
    :initform "")
   (req_confirm
    :reader req_confirm
    :initarg :req_confirm
    :type cl:boolean
    :initform cl:nil))
)

(cl:defclass UIMessage (<UIMessage>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <UIMessage>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'UIMessage)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name QRcode_detection-msg:<UIMessage> is deprecated: use QRcode_detection-msg:UIMessage instead.")))

(cl:ensure-generic-function 'id-val :lambda-list '(m))
(cl:defmethod id-val ((m <UIMessage>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader QRcode_detection-msg:id-val is deprecated.  Use QRcode_detection-msg:id instead.")
  (id m))

(cl:ensure-generic-function 'timeout-val :lambda-list '(m))
(cl:defmethod timeout-val ((m <UIMessage>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader QRcode_detection-msg:timeout-val is deprecated.  Use QRcode_detection-msg:timeout instead.")
  (timeout m))

(cl:ensure-generic-function 'msg-val :lambda-list '(m))
(cl:defmethod msg-val ((m <UIMessage>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader QRcode_detection-msg:msg-val is deprecated.  Use QRcode_detection-msg:msg instead.")
  (msg m))

(cl:ensure-generic-function 'req_confirm-val :lambda-list '(m))
(cl:defmethod req_confirm-val ((m <UIMessage>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader QRcode_detection-msg:req_confirm-val is deprecated.  Use QRcode_detection-msg:req_confirm instead.")
  (req_confirm m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <UIMessage>) ostream)
  "Serializes a message object of type '<UIMessage>"
  (cl:let* ((signed (cl:slot-value msg 'id)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'timeout)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let ((__ros_str_len (cl:length (cl:slot-value msg 'msg))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_str_len) ostream))
  (cl:map cl:nil #'(cl:lambda (c) (cl:write-byte (cl:char-code c) ostream)) (cl:slot-value msg 'msg))
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'req_confirm) 1 0)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <UIMessage>) istream)
  "Deserializes a message object of type '<UIMessage>"
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'id) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'timeout) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((__ros_str_len 0))
      (cl:setf (cl:ldb (cl:byte 8 0) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'msg) (cl:make-string __ros_str_len))
      (cl:dotimes (__ros_str_idx __ros_str_len msg)
        (cl:setf (cl:char (cl:slot-value msg 'msg) __ros_str_idx) (cl:code-char (cl:read-byte istream)))))
    (cl:setf (cl:slot-value msg 'req_confirm) (cl:not (cl:zerop (cl:read-byte istream))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<UIMessage>)))
  "Returns string type for a message object of type '<UIMessage>"
  "QRcode_detection/UIMessage")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'UIMessage)))
  "Returns string type for a message object of type 'UIMessage"
  "QRcode_detection/UIMessage")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<UIMessage>)))
  "Returns md5sum for a message object of type '<UIMessage>"
  "093e3e003e515d1fb001a7a552ea63e5")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'UIMessage)))
  "Returns md5sum for a message object of type 'UIMessage"
  "093e3e003e515d1fb001a7a552ea63e5")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<UIMessage>)))
  "Returns full string definition for message of type '<UIMessage>"
  (cl:format cl:nil "int32 id~%int32 timeout~%string msg~%bool req_confirm~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'UIMessage)))
  "Returns full string definition for message of type 'UIMessage"
  (cl:format cl:nil "int32 id~%int32 timeout~%string msg~%bool req_confirm~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <UIMessage>))
  (cl:+ 0
     4
     4
     4 (cl:length (cl:slot-value msg 'msg))
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <UIMessage>))
  "Converts a ROS message object to a list"
  (cl:list 'UIMessage
    (cl:cons ':id (id msg))
    (cl:cons ':timeout (timeout msg))
    (cl:cons ':msg (msg msg))
    (cl:cons ':req_confirm (req_confirm msg))
))
