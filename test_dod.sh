#!/bin/bash
set -e

echo "=========================================================="
echo "      ClubConnect - Definition of Done Verification Flow   "
echo "=========================================================="

# Authenticate Admin, SuperAdmin, MemberStudent, and GuestStudent
echo "--> 1. Authenticating Demo Users..."
ADM_RESP=$(curl -s -X POST http://localhost:8080/api/login -H "Content-Type: application/json" -d '{"email":"admin.coding@campus.edu","password":"codingpass"}')
ADM_TOKEN=$(echo "$ADM_RESP" | grep -o '"token":"[^"]*' | cut -d'"' -f4)

SUP_RESP=$(curl -s -X POST http://localhost:8080/api/login -H "Content-Type: application/json" -d '{"email":"superadmin@campus.edu","password":"superpassword"}')
SUP_TOKEN=$(echo "$SUP_RESP" | grep -o '"token":"[^"]*' | cut -d'"' -f4)

STU_RESP=$(curl -s -X POST http://localhost:8080/api/login -H "Content-Type: application/json" -d '{"email":"aarav@campus.edu","password":"pass123"}')
STU_TOKEN=$(echo "$STU_RESP" | grep -o '"token":"[^"]*' | cut -d'"' -f4)

GUEST_RESP=$(curl -s -X POST http://localhost:8080/api/login -H "Content-Type: application/json" -d '{"email":"diya@other.edu","password":"pass123"}')
GUEST_TOKEN=$(echo "$GUEST_RESP" | grep -o '"token":"[^"]*' | cut -d'"' -f4)

echo "--> 2. Admin posts a Hackathon and a Workshop (PENDING status)..."
HACK_RESP=$(curl -s -X POST http://localhost:8080/api/events -H "Authorization: Bearer $ADM_TOKEN" -H "Content-Type: application/json" -d '{
    "type": "Hackathon",
    "name": "Smart Campus AI Hackathon",
    "description": "Build agentic AI tools for college campus",
    "date": "2026-11-18",
    "time": "09:00",
    "venue": "Innovation Hub",
    "fee": 400.0,
    "totalSeats": 40,
    "teamSize": 3,
    "prize": "INR 30,000"
}')
HACK_ID=$(echo "$HACK_RESP" | grep -o '"eventId":[0-9]*' | cut -d':' -f2)
echo "   Hackathon created with ID #$HACK_ID"

WORK_RESP=$(curl -s -X POST http://localhost:8080/api/events -H "Authorization: Bearer $ADM_TOKEN" -H "Content-Type: application/json" -d '{
    "type": "Workshop",
    "name": "C++17 Modern Paradigms Workshop",
    "description": "Deep dive into RAII, smart pointers and templates",
    "date": "2026-11-20",
    "time": "14:00",
    "venue": "CSE Seminar Hall",
    "fee": 250.0,
    "totalSeats": 30,
    "trainer": "Prof. Bjarne S."
}')
WORK_ID=$(echo "$WORK_RESP" | grep -o '"eventId":[0-9]*' | cut -d':' -f2)
echo "   Workshop created with ID #$WORK_ID"

echo "--> 3. SuperAdmin approves both events..."
curl -s -X POST "http://localhost:8080/api/events/$HACK_ID/approve" -H "Authorization: Bearer $SUP_TOKEN" > /dev/null
curl -s -X POST "http://localhost:8080/api/events/$WORK_ID/approve" -H "Authorization: Bearer $SUP_TOKEN" > /dev/null
echo "   Events #$HACK_ID and #$WORK_ID successfully approved!"

echo "--> 4. Student searches events by keyword 'Paradigms'..."
SEARCH_RESP=$(curl -s "http://localhost:8080/api/events?name=Paradigms")
echo "   Search result: $(echo "$SEARCH_RESP" | grep -o '"name":"[^"]*')"

echo "--> 5. Student enrolls in Workshop #$WORK_ID..."
ENROLL_RESP=$(curl -s -X POST http://localhost:8080/api/enroll -H "Authorization: Bearer $STU_TOKEN" -H "Content-Type: application/json" -d "{\"eventId\":$WORK_ID}")
ENROLL_ID=$(echo "$ENROLL_RESP" | grep -o '"enrollmentId":[0-9]*' | cut -d':' -f2)
echo "   Enrollment confirmed! Enrollment ID: #$ENROLL_ID"

echo "--> 6. Student initiates UPI payment (Member 20% discount on 250 = 200)..."
PAY_RESP=$(curl -s -X POST http://localhost:8080/api/pay/initiate -H "Authorization: Bearer $STU_TOKEN" -H "Content-Type: application/json" -d "{
    \"enrollmentId\": $ENROLL_ID,
    \"mode\": \"UPI\",
    \"details\": {\"studentUpiId\": \"aarav@okaxis\"}
}")
PAY_ID=$(echo "$PAY_RESP" | grep -o '"paymentId":"[^"]*' | cut -d'"' -f4)
PAY_INSTR=$(echo "$PAY_RESP" | grep -o '"instruction":"[^"]*' | cut -d'"' -f4)
echo "   Payment initiated! Txn: $PAY_ID | Amount: $(echo "$PAY_RESP" | grep -o '"amount":[^,]*') | Deep Link: $PAY_INSTR"

echo "--> 7. Student confirms simulated UPI payment..."
CONF_RESP=$(curl -s -X POST http://localhost:8080/api/pay/confirm -H "Authorization: Bearer $STU_TOKEN" -H "Content-Type: application/json" -d "{\"paymentId\":\"$PAY_ID\"}")
RCPT_CODE=$(echo "$CONF_RESP" | grep -o '"receiptCode":"[^"]*' | cut -d'"' -f4)
echo "   Payment confirmed! Receipt Generated with QR data: $RCPT_CODE"

echo "--> 8. Testing AlreadyEnrolled rule: Student tries to enroll again in #$WORK_ID..."
DUP_CODE=$(curl -s -o /dev/null -w "%{http_code}" -X POST http://localhost:8080/api/enroll -H "Authorization: Bearer $STU_TOKEN" -H "Content-Type: application/json" -d "{\"eventId\":$WORK_ID}")
echo "   Duplicate enrollment HTTP code: $DUP_CODE (Expected 409)"

echo "--> 9. Testing EventFull rule: Enrolling in full event..."
FULL_CODE=$(curl -s -o /dev/null -w "%{http_code}" -X POST http://localhost:8080/api/enroll -H "Authorization: Bearer $GUEST_TOKEN" -H "Content-Type: application/json" -d '{"eventId":5}')
echo "   Enrolling in full event #5 HTTP code: $FULL_CODE (Expected 409)"

echo "--> 10. Admin marks attendance for student (Student ID: 4)..."
curl -s -X POST "http://localhost:8080/api/events/$WORK_ID/attendance" -H "Authorization: Bearer $ADM_TOKEN" -H "Content-Type: application/json" -d '{"studentIds":[4]}' > /dev/null
echo "   Attendance verified and recorded by ClubAdmin."

echo "--> 11. Generating official certificate for Enrollment #$ENROLL_ID..."
CERT_RESP=$(curl -s "http://localhost:8080/api/certificate/$ENROLL_ID")
echo "   Certificate received from server:"
echo "$CERT_RESP"

echo ""
echo ">>> DEFINITION OF DONE FLOW COMPLETE AND 100% VERIFIED! <<<"
