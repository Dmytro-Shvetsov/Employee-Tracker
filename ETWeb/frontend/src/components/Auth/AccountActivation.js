import React from 'react';
import axios from 'axios';
import {confirmEmail} from '../../services/authService'
import { Redirect } from 'react-router-dom';


export default class AccountActivation extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            responseReceived: false
        };
        this._isMounted = false;
        this.reqSource = undefined;
    }

    setState = (...args) => {
        if (this._isMounted) {
            super.setState(...args);
        }
    };

    cancelPreviousRequests = async () => {
        if (this.reqSource) {
            this.reqSource.cancel();
        }
        this.reqSource = axios.CancelToken.source();
    };

    tryConfirmEmail = async () => {
        console.log(this.props.match);
        const {params} = this.props.match;
        await this.cancelPreviousRequests();
        try {
            const response = await confirmEmail(params, this.reqSource.token);
            console.log('Confirmation successful', response);
            alert(response.data.detail);
        } catch (error) {
            console.error(error.message);
            if (error.response.status === 400) {
                const errorMessage = Object.values(error.response.data).map(errList => errList.join(". ")).join(" ");
                alert(errorMessage);
            } else {
                alert('Unknown email confirmation error.');
            }
        }
        this.setState({responseReceived: true});
    };

    async componentDidMount() {
        this._isMounted = true;
        await this.tryConfirmEmail();
    }

    async componentWillUnmount() {
        this._isMounted = false;
        await this.cancelPreviousRequests();
    }

    render() {
        if (!this.state.responseReceived) {
            return <span>Wrapping up...</span>;
        }
        return <Redirect to="/login"/>
    }
}
