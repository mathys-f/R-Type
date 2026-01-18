/* Import modules */
import bcrypt from 'bcryptjs';
import jwt from 'jsonwebtoken';
import { Account } from '../model/indexModel.js';

/* Controller for account registration */
const register = async (req, res) => {
    const { email, password, username } = req.body;

    if (!email || !password || !username) {
        return res.status(400).json({ success: false, error: 'Email, password and username are required' });
    }

    const existingAccount = await Account.findOne({ where: { email } });
    if (existingAccount) {
        return res.status(409).json({ success: false, error: 'Account with this email already exists' });
    }

    const password_hash = await bcrypt.hash(password, 10);
    const newAccount = await Account.create({ email, password_hash, username });
    const token = jwt.sign({ id: newAccount.id }, process.env.JWT_SECRET, { expiresIn: '7d' });

    res.status(201).json({ 
        success: true, 
        message: 'Account registered successfully',
        token,
        user: { id: newAccount.id, email: newAccount.email, username: newAccount.username }
    });
}

/* Controller for account login */
const login = async (req, res) => {
    const { email, password } = req.body;

    if (!email || !password) {
        return res.status(400).json({ success: false, error: 'Email and password are required' });
    }

    const account = await Account.findOne({ where: { email } });
    if (!account || !(await bcrypt.compare(password, account.password_hash))) {
        return res.status(401).json({ success: false, error: 'Invalid email or password' });
    }

    // Update last_login
    account.last_login = new Date();
    await account.save();

    const token = jwt.sign({ id: account.id }, process.env.JWT_SECRET, { expiresIn: '7d' });

    res.status(200).json({ 
        success: true, 
        message: 'Login successful',
        token,
        user: { id: account.id, email: account.email, username: account.username }
    });
};

/* Controller for account logout */
const logout = (req, res) => {
    const token = req.cookies.token;
    if (!token) {
        return res.status(401).json({ success: false, message: 'Not authenticated' });
    }
    res.clearCookie('token');
    res.status(200).json({ success: true, message: 'Account logged out successfully' });
};

/* Controller to check if account is connected */
const isConnected = (req, res) => {
    const token = req.cookies.token;
    if (!token) {
        return res.status(401).json({ success: false, connected: false });
    }
    try {
        jwt.verify(token, process.env.JWT_SECRET);
        return res.status(200).json({ success: true, connected: true });
    } catch (err) {
        return res.status(401).json({ success: false, connected: false });
    }
};

/* Exported controllers */
export default { register, login, logout, isConnected };
