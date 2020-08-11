import React from 'react';
import * as auth from '../../../services/authService';
import * as utils from '../../../utils'
import {
    Button,
    Form,
    Spinner
} from 'reactstrap';
import { SaveOutlined, EditOutlined, CloseSquareOutlined } from '@ant-design/icons';
import { Input } from '../../common';
import axios from "axios";


export default class UserProfile extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            user: props.user,
            savedData: {},
            unsavedData: {},
            errors: {},
            editing: false
        };
        this.reqSource = undefined;
        this._isMounted = false;
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

    componentDidMount() {
        this._isMounted = true;
        const { user: { email } } = this.state;

        this.setState({
            unsavedData: { email },
            savedData: { email }
        });
    }

    async componentWillUnmount() {
        this._isMounted = false;
        await this.cancelPreviousRequests();
    }

    handleTextInputChange = event => {
        const target = event.target;
        const name = target.name;

        this.setState({
            unsavedData: { ...this.state.unsavedData, [name]: target.value},
            errors: {...this.state.errors, [name]: undefined},
        });
    };

    handleAccountUpdate = async event => {
        event.preventDefault();
        await this.cancelPreviousRequests();
        const {user, unsavedData} = this.state;
        try {
            const response = await auth.updateUserAccount({...unsavedData, user: {...user}}, this.reqSource.token);
            const data = JSON.parse(response.data);
            this.setState({
                savedData: {...data},
                unsavedData: {...data},
                editing: false
            });
        } catch (error) {
            if (error.response.status === 400) {
                const fieldErrors = error.response.data;
                Object.keys(fieldErrors).map((fieldName) => {
                    fieldErrors[fieldName] = fieldErrors[fieldName].join(" ");
                    console.log(fieldErrors[fieldName]);
                });

                this.setState({
                    errors: fieldErrors
                });
            }
        }
    };

    resetForm() {
        const { savedData } = this.state;
        this.setState({
            unsavedData: {...savedData},
            errors: {}
        });
    }

    renderForm() {
        const { editing, unsavedData, errors } = this.state;

        return (
            <Form className="row">
                <div className="col-md-5">
                    {Object.keys(unsavedData).map(key => (
                        <Input
                            disabled={editing === false}
                            labelText={key.split("_").map(utils.capitalize).join(" ")}
                            value={unsavedData[key] || ''}
                            name={key}
                            onChange={this.handleTextInputChange}
                            required={false}
                            key={key}
                            error={errors[key]}
                        />
                    ))}
                </div>
            </Form>

        );
    }

    render() {
        const { savedData, editing } = this.state;
        if (Object.keys(savedData).length === 0) {
            return <Spinner/>
        }
        return (
            <div className="row">
                <div className="col-md-12">
                    <div className="card card-primary card-outline">
                        <div className="card-body pt-3">
                            {this.renderForm()}
                        </div>

                        <div className="card-footer d-flex justify-content-end">
                            {editing ? (
                                <React.Fragment>
                                    <Button color="success"
                                        className="noselect d-flex align-items-center mr-2"
                                        onClick={this.handleAccountUpdate}
                                    >
                                        <SaveOutlined/>
                                        Update Account
                                    </Button>
                                    <Button
                                        className="d-flex align-items-center"
                                        onClick={e => { this.resetForm(); this.setState({editing: false}) }}
                                    >
                                        <CloseSquareOutlined />
                                        Cancel
                                    </Button>
                                </React.Fragment>
                            ) : (
                                <React.Fragment>
                                    <Button
                                        className="d-flex align-items-center"
                                        onClick={e => { this.setState({editing: true}); }}
                                    >
                                        <EditOutlined/>
                                        Edit
                                    </Button>
                                </React.Fragment>
                            )}

                        </div>
                    </div>
                </div>
            </div>
        );
    }
};