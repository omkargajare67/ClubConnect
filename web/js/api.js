// ClubConnect - Centralized API Service
// Encapsulates all HTTP communications with C++17 backend

const API = {
    BASE_URL: '',

    // Token & User Management in LocalStorage
    getToken() {
        return localStorage.getItem('cc_token') || '';
    },

    getUser() {
        try {
            return JSON.parse(localStorage.getItem('cc_user')) || null;
        } catch {
            return null;
        }
    },

    setAuth(token, user) {
        localStorage.setItem('cc_token', token);
        localStorage.setItem('cc_user', JSON.stringify(user));
    },

    clearAuth() {
        localStorage.removeItem('cc_token');
        localStorage.removeItem('cc_user');
    },

    logout() {
        this.clearAuth();
        window.location.href = 'index.html';
    },

    // Core HTTP Request Wrapper
    async request(endpoint, options = {}) {
        const url = `${this.BASE_URL}${endpoint}`;
        const headers = options.headers || {};

        if (!headers['Content-Type'] && !(options.body instanceof FormData)) {
            headers['Content-Type'] = 'application/json';
        }

        const token = this.getToken();
        if (token) {
            headers['Authorization'] = `Bearer ${token}`;
        }

        const config = {
            ...options,
            headers
        };

        const response = await fetch(url, config);
        const data = await response.json().catch(() => ({}));

        if (!response.ok) {
            const error = new Error(data.error || `HTTP error ${response.status}`);
            error.status = response.status;
            error.data = data;
            throw error;
        }

        return data;
    },

    // 1. Authentication
    async login(email, password) {
        const data = await this.request('/api/login', {
            method: 'POST',
            body: JSON.stringify({ email, password })
        });
        this.setAuth(data.token, data);
        return data;
    },

    async register(userData) {
        const data = await this.request('/api/register', {
            method: 'POST',
            body: JSON.stringify(userData)
        });
        this.setAuth(data.token, data);
        return data;
    },

    async changePassword(oldPassword, newPassword) {
        return this.request('/api/user/change-password', {
            method: 'POST',
            body: JSON.stringify({ oldPassword, newPassword })
        });
    },

    // 2. Events & Queries
    async getEvents(params = {}) {
        const query = new URLSearchParams();
        if (params.name) query.set('name', params.name);
        if (params.club) query.set('club', params.club);
        if (params.maxFee) query.set('maxFee', params.maxFee);
        if (params.sort) query.set('sort', params.sort);
        const qs = query.toString() ? `?${query.toString()}` : '';
        return this.request(`/api/events${qs}`);
    },

    async getPendingEvents() {
        return this.request('/api/events/pending');
    },

    async createEvent(eventData) {
        return this.request('/api/events', {
            method: 'POST',
            body: JSON.stringify(eventData)
        });
    },

    async approveEvent(eventId) {
        return this.request(`/api/events/${eventId}/approve`, {
            method: 'POST'
        });
    },

    async rejectEvent(eventId) {
        return this.request(`/api/events/${eventId}/reject`, {
            method: 'POST'
        });
    },

    // 3. Student Enrollment & Payment
    async enroll(eventId) {
        return this.request('/api/enroll', {
            method: 'POST',
            body: JSON.stringify({ eventId })
        });
    },

    async initiatePayment(enrollmentId, mode, details = {}) {
        return this.request('/api/pay/initiate', {
            method: 'POST',
            body: JSON.stringify({ enrollmentId, mode, details })
        });
    },

    async confirmPayment(paymentId) {
        return this.request('/api/pay/confirm', {
            method: 'POST',
            body: JSON.stringify({ paymentId })
        });
    },

    async getMyEnrollments() {
        return this.request('/api/my/enrollments');
    },

    async getReceipt(receiptId) {
        return this.request(`/api/receipt/${receiptId}`);
    },

    // 4. Attendance & Certificates
    async getAttendees(eventId) {
        return this.request(`/api/events/${eventId}/attendees`);
    },

    async markAttendance(eventId, studentIds) {
        return this.request(`/api/events/${eventId}/attendance`, {
            method: 'POST',
            body: JSON.stringify({ studentIds })
        });
    },

    async getCertificate(enrollmentId) {
        return this.request(`/api/certificate/${enrollmentId}`);
    },

    // 5. Reports
    async getReports() {
        return this.request('/api/reports');
    },

    // 6. Clubs
    async getClubs() {
        return this.request('/api/clubs');
    },

    async createClub(clubData) {
        return this.request('/api/clubs', {
            method: 'POST',
            body: JSON.stringify(clubData)
        });
    }
};
