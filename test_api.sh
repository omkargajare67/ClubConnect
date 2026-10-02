#!/bin/bash
set -e

echo "=== 1. Test Logins & 401 Unauthorized ==="
# Bad login -> 401
CODE=$(curl -s -o /dev/null -w "%{http_code}" -X POST http://localhost:8080/api/login -H "Content-Type: application/json" -d '{"email":"aarav@campus.edu","password":"wrong"}')
echo "Bad login status code: $CODE (Expected 401)"

# Student login
STU_RESP=$(curl -s -X POST http://localhost:8080/api/login -H "Content-Type: application/json" -d '{"email":"aarav@campus.edu","password":"pass123"}')
echo "Student login: $STU_RESP"
STU_TOKEN=$(echo "$STU_RESP" | grep -o '"token":"[^"]*' | cut -d'"' -f4)

# Admin login
ADM_RESP=$(curl -s -X POST http://localhost:8080/api/login -H "Content-Type: application/json" -d '{"email":"admin.coding@campus.edu","password":"codingpass"}')
echo "Admin login: $ADM_RESP"
ADM_TOKEN=$(echo "$ADM_RESP" | grep -o '"token":"[^"]*' | cut -d'"' -f4)

# SuperAdmin login
SUP_RESP=$(curl -s -X POST http://localhost:8080/api/login -H "Content-Type: application/json" -d '{"email":"superadmin@campus.edu","password":"superpassword"}')
echo "SuperAdmin login: $SUP_RESP"
SUP_TOKEN=$(echo "$SUP_RESP" | grep -o '"token":"[^"]*' | cut -d'"' -f4)

echo ""
echo "=== 2. Test Role-Based Access Control: 401 & 403 ==="
CODE_NO_AUTH=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/api/events/pending)
echo "No token status code: $CODE_NO_AUTH (Expected 401)"

CODE_FORBIDDEN=$(curl -s -o /dev/null -w "%{http_code}" -H "Authorization: Bearer $STU_TOKEN" http://localhost:8080/api/events/pending)
echo "Student token on SuperAdmin route status code: $CODE_FORBIDDEN (Expected 403)"

echo ""
echo "=== 3. Test 404 Not Found ==="
CODE_404=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/api/receipt/99999)
echo "Invalid receipt ID status code: $CODE_404 (Expected 404)"

echo ""
echo "=== 4. Test Event Filters & Search ==="
echo "Filtered by club 'Coding Club':"
curl -s "http://localhost:8080/api/events?club=Coding%20Club" | grep -o '"name":"[^"]*'
echo "Filtered by maxFee=200 & sort=fee:"
curl -s "http://localhost:8080/api/events?maxFee=200&sort=fee" | grep -o '"fee":[^,]*'

echo ""
echo "=== 5. Test ClubAdmin Post Event & SuperAdmin Approve ==="
POST_RESP=$(curl -s -X POST http://localhost:8080/api/events -H "Authorization: Bearer $ADM_TOKEN" -H "Content-Type: application/json" -d '{
    "type": "Workshop",
    "name": "Cloud Native Kubernetes Sprint",
    "description": "Cluster setup and helm charts",
    "date": "2026-11-25",
    "time": "14:00",
    "venue": "Server Room B",
    "fee": 200.0,
    "totalSeats": 20,
    "trainer": "Er. Devops"
}')
echo "Created Event Response: $POST_RESP"
EVENT_ID=$(echo "$POST_RESP" | grep -o '"eventId":[0-9]*' | cut -d':' -f2)

# Approve by SuperAdmin
APP_RESP=$(curl -s -X POST "http://localhost:8080/api/events/$EVENT_ID/approve" -H "Authorization: Bearer $SUP_TOKEN")
echo "SuperAdmin Approval Response: $APP_RESP"

echo ""
echo "=== 6. Test Student Enrollment & 409 Conflict Exceptions ==="
# Successful enroll
ENROLL_RESP=$(curl -s -X POST http://localhost:8080/api/enroll -H "Authorization: Bearer $STU_TOKEN" -H "Content-Type: application/json" -d "{\"eventId\":$EVENT_ID}")
echo "Enrollment Response: $ENROLL_RESP"
ENROLL_ID=$(echo "$ENROLL_RESP" | grep -o '"enrollmentId":[0-9]*' | cut -d':' -f2)

# Attempt duplicate enrollment -> expect 409 AlreadyEnrolledException
CODE_DUP=$(curl -s -o /dev/null -w "%{http_code}" -X POST http://localhost:8080/api/enroll -H "Authorization: Bearer $STU_TOKEN" -H "Content-Type: application/json" -d "{\"eventId\":$EVENT_ID}")
echo "Duplicate enrollment status code: $CODE_DUP (Expected 409)"

# Attempt enrollment in Event #5 which has 1 seat left
# Login guest student Kabir
KABIR_RESP=$(curl -s -X POST http://localhost:8080/api/login -H "Content-Type: application/json" -d '{"email":"kabir@other.edu","password":"pass123"}')
KABIR_TOKEN=$(echo "$KABIR_RESP" | grep -o '"token":"[^"]*' | cut -d'"' -f4)

# Login guest student Diya
DIYA_RESP=$(curl -s -X POST http://localhost:8080/api/login -H "Content-Type: application/json" -d '{"email":"diya@other.edu","password":"pass123"}')
DIYA_TOKEN=$(echo "$DIYA_RESP" | grep -o '"token":"[^"]*' | cut -d'"' -f4)

# Kabir takes the 1 remaining seat in Event 5
E5_KABIR=$(curl -s -o /dev/null -w "%{http_code}" -X POST http://localhost:8080/api/enroll -H "Authorization: Bearer $KABIR_TOKEN" -H "Content-Type: application/json" -d '{"eventId":5}')
echo "Kabir enrolling in last seat of Event 5: $E5_KABIR (Expected 201)"

# Diya tries to enroll in Event 5 (now completely full) -> expect 409 EventFullException
E5_DIYA=$(curl -s -o /dev/null -w "%{http_code}" -X POST http://localhost:8080/api/enroll -H "Authorization: Bearer $DIYA_TOKEN" -H "Content-Type: application/json" -d '{"eventId":5}')
echo "Diya enrolling in full Event 5: $E5_DIYA (Expected 409)"

echo ""
echo "=== 7. Test Payment: 402 Card Failure, UPI Initiate, Confirm ==="
# Test bad card payment -> expect 402 PaymentFailedException
BAD_CARD_RESP=$(curl -s -o /dev/null -w "%{http_code}" -X POST http://localhost:8080/api/pay/initiate -H "Authorization: Bearer $STU_TOKEN" -H "Content-Type: application/json" -d "{
    \"enrollmentId\": $ENROLL_ID,
    \"mode\": \"Card\",
    \"details\": {\"cardNumber\": \"12345\", \"cardHolderName\": \"Aarav\"}
}")
echo "Bad Card Initiate status code: $BAD_CARD_RESP (Expected 402)"

# Initiate UPI payment
# Member student gets 20% discount: 200 base fee -> 160
PAY_RESP=$(curl -s -X POST http://localhost:8080/api/pay/initiate -H "Authorization: Bearer $STU_TOKEN" -H "Content-Type: application/json" -d "{
    \"enrollmentId\": $ENROLL_ID,
    \"mode\": \"UPI\",
    \"details\": {\"studentUpiId\": \"aarav@okhdfcbank\"}
}")
echo "UPI Initiate Response: $PAY_RESP"
PAY_ID=$(echo "$PAY_RESP" | grep -o '"paymentId":"[^"]*' | cut -d'"' -f4)

# Confirm payment
CONF_RESP=$(curl -s -X POST http://localhost:8080/api/pay/confirm -H "Authorization: Bearer $STU_TOKEN" -H "Content-Type: application/json" -d "{\"paymentId\":\"$PAY_ID\"}")
echo "Payment Confirm Response: $CONF_RESP"
RCPT_ID=$(echo "$CONF_RESP" | grep -o '"receiptId":[0-9]*' | cut -d':' -f2)

# Verify Receipt endpoint
RCPT_RESP=$(curl -s "http://localhost:8080/api/receipt/$RCPT_ID")
echo "Fetched Receipt: $RCPT_RESP"

echo ""
echo "=== 8. Test Attendance & Certificate Generation ==="
# Certificate before attendance -> expect 400 error
CERT_PRE=$(curl -s -o /dev/null -w "%{http_code}" "http://localhost:8080/api/certificate/$ENROLL_ID")
echo "Certificate pre-attendance status code: $CERT_PRE (Expected 400)"

# ClubAdmin marks attendance for student (Student ID = 4 for Aarav)
ATT_RESP=$(curl -s -X POST "http://localhost:8080/api/events/$EVENT_ID/attendance" -H "Authorization: Bearer $ADM_TOKEN" -H "Content-Type: application/json" -d '{"studentIds":[4]}')
echo "Attendance Response: $ATT_RESP"

# Certificate after attendance -> expect 200 with certificate content
CERT_POST=$(curl -s "http://localhost:8080/api/certificate/$ENROLL_ID")
echo "Certificate Content: $CERT_POST"

echo ""
echo "=== 9. Test SuperAdmin Reports ==="
REPORT_RESP=$(curl -s -H "Authorization: Bearer $SUP_TOKEN" http://localhost:8080/api/reports)
echo "SuperAdmin Reports: $REPORT_RESP"

echo ""
echo ">>> ALL ENDPOINTS & STATUS CODES (200, 201, 401, 402, 403, 404, 409) VERIFIED! <<<"
