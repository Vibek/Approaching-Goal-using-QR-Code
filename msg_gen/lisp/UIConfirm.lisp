; Auto-generated. Do not edit!


(cl:in-package QRcode_detection-msg)


;//! \htmlinclude UIConfirm.msg.html

(cl:defclass <UIConfirm> (roslisp-msg-protocol:ros-message)
  ((id
    :reader id
    :initarg :id
    :type cl:integer
    :initform 0)
   (confirmed
    :reader confirmed
    :initarg :confirmed
    :type cl:boolean
    :initform cl:nil))
)

(cl:defclass UIConfirm (<UIConfirm>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <UIConfirm>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'UIConfirm)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name QRcode_detection-msg:<UIConfirm> is deprecated: use QRcode_detection-msg:UIConfirm instead.")))

(cl:ensure-generic-function 'id-val :lambda-list '(m))
(cl:defmethod id-val ((m <UIConfirm>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader QRcode_detection-msg:id-val is deprecated.  Use QRcode_detection-msg:id instead.")
  (id m))

(cl:ensure-generic-function 'confirmed-val :lambda-list '(m))
(cl:defmethod confirmed-val ((m <UIConfirm>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader QRcode_detection-msg:confirmed-val is deprecated.  Use QRcode_detection-msg:confirmed instead.")
  (confirmed m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <UIConfirm>) ostream)
  "Serializes a message object of type '<UIConfirm>"
  (cl:let* ((signed (cl:slot-value msg 'id)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'confirmed) 1 0)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <UIConfirm>) istream)
  "Deserializes a message object of type '<UIConfirm>"
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'id) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:setf (cl:slot-value msg 'confirmed) (cl:not (cl:zerop (cl:read-byte istream))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<UIConfirm>)))
  "Returns string type for a message object of type '<UIConfirm>"
  "QRcode_detection/UIConfirm")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'UIConfirm)))
  "Returns string type for a message object of type 'UIConfirm"
  "QRcode_detection/UIConfirm")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<UIConfirm>)))
  "Returns md5sum for a message object of type '<UIConfirm>"
  "86a9b286e1d2e325349676b33c0197a6")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'UIConfirm)))
  "Returns md5sum for a message object of type 'UIConfirm"
  "86a9b286e1d2e325349676b33c0197a6")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<UIConfirm>)))
  "Returns full string definition for message of type '<UIConfirm>"
  (cl:format cl:nil "int32 id~%bool confirmed~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'UIConfirm)))
  "Returns full string definition for message of type 'UIConfirm"
  (cl:format cl:nil "int32 id~%bool confirmed~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <UIConfirm>))
  (cl:+ 0
     4
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <UIConfirm>))
  "Converts a ROS message object to a list"
  (cl:list 'UIConfirm
    (cl:cons ':id (id msg))
    (cl:cons ':confirmed (confirmed msg))
))
