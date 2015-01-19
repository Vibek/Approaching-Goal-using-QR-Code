
(cl:in-package :asdf)

(defsystem "QRcode_detection-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :sensor_msgs-msg
               :std_msgs-msg
)
  :components ((:file "_package")
    (:file "UIConfirm" :depends-on ("_package_UIConfirm"))
    (:file "_package_UIConfirm" :depends-on ("_package"))
    (:file "Pose" :depends-on ("_package_Pose"))
    (:file "_package_Pose" :depends-on ("_package"))
    (:file "Markers" :depends-on ("_package_Markers"))
    (:file "_package_Markers" :depends-on ("_package"))
    (:file "Goal" :depends-on ("_package_Goal"))
    (:file "_package_Goal" :depends-on ("_package"))
    (:file "UIMessage" :depends-on ("_package_UIMessage"))
    (:file "_package_UIMessage" :depends-on ("_package"))
    (:file "Landmark" :depends-on ("_package_Landmark"))
    (:file "_package_Landmark" :depends-on ("_package"))
    (:file "PerceptInfo" :depends-on ("_package_PerceptInfo"))
    (:file "_package_PerceptInfo" :depends-on ("_package"))
  ))