import React from 'react';
import {
    Button,
    Form,
    Spinner
} from 'reactstrap';

import { SaveOutlined, EditOutlined, CloseSquareOutlined } from '@ant-design/icons';
import { Input } from '../common/index';
import { getUserProfile, updateUserProfile } from '../../services/authService';
import * as utils from '../../utils'


export default class UserProfile extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            user: props.user,
            savedData: {},
            unsavedData: {},
            errors: {},
            editing: false
        }
    }

    loadProfileData() {
        const { user } = this.state;
        getUserProfile(user).then(res => {
            const data = JSON.parse(res.data);
            this.setState({
                savedData: {...data },
                unsavedData: { ...data }
            });
        }).catch(error => {
            console.error(error);
            console.error(error.response.data);

            switch (error.response.status) {
                case 400: {
                    const fieldErrors = error.response.data;
                    Object.keys(fieldErrors).map((fieldName) => {
                        fieldErrors[fieldName] = fieldErrors[fieldName].join(" ");
                    });

                    this.setState({
                        errors: fieldErrors
                    });
                    break;
                }
                case 401: {
                    window.location.replace("/login");
                    break;
                }
                default: {
                    console.log("Unexpected error occurred. ", error);
                }
            }
        });
    }

    componentDidMount() {
        this.loadProfileData();
    }

    handleTextInputChange = event => {
        const target = event.target;
        const name = target.name;

        this.setState({
            unsavedData: { ...this.state.unsavedData, [name]: target.value},
            errors: {...this.state.errors, [name]: undefined},
        });
    };

    handleFileInputChange = event => {
        const target = event.target;
        const name = target.name;

        this.setState({
            unsavedData: { ...this.state.unsavedData, [name]: target.files[0]},
            errors: {...this.state.errors, [name]: undefined},
        });
    };

    handleProfileUpdate = event => {
        event.preventDefault();
        const { user, unsavedData } = this.state;
        if (utils.typeOf(unsavedData.image) === "string") {
            delete unsavedData.image;
        }
        updateUserProfile(user, unsavedData).then(res => {
            const data = JSON.parse(res.data);

            this.setState({
                savedData: {...data },
                unsavedData: { ...data },
                editing: false
            });
        }).catch(error => {
            console.error(error);
            console.error(error.response.data);

            switch (error.response.status) {
                case 400: {
                    const fieldErrors = error.response.data;
                    Object.keys(fieldErrors).map((fieldName) => {
                        fieldErrors[fieldName] = fieldErrors[fieldName].join(" ");
                        console.log(fieldErrors[fieldName]);
                    });

                    this.setState({
                        errors: fieldErrors
                    });
                    break;
                }
                case 401: {
                    window.location.replace("/login");
                    break;
                }
                default: {
                    console.log("Unexpected error occurred. ", error);
                }
            }
        });
    };

    resetForm() {
        const { savedData } = this.state;
        this.setState({
            unsavedData: {...savedData},
            errors: {}
        });
    }

    renderForm() {
        const { editing, unsavedData: { image, ...rest }, errors } = this.state;
        return (
            <Form className="row">
                <div className="col-md-6">
                    {Object.keys(rest).map(key => (
                        <Input
                            disabled={editing === false}
                            labelText={key.split("_").map(utils.capitalize).join(" ")}
                            value={rest[key] || ''}
                            name={key}
                            onChange={this.handleTextInputChange}
                            required={false}
                            key={key}
                            error={errors[key]}
                        />
                    ))}
                </div>
                <div className="col-md-6">
                    <div id="profile-image-input">
                        <img
                            src={this.state.savedData.image}
                            id="profile-image"
                            alt="Your profile image."
                            onClick={e => { document.getElementById("image").click();}}
                        />
                        <Input
                            disabled={editing === false}
                            type="file"
                            name="image"
                            accept="image/*"
                            onChange={this.handleFileInputChange}
                            error={errors.image}
                        />
                    </div>
                </div>
            </Form>

        );
    }

    render() {
        const { user, savedData, editing } = this.state;
        if (Object.keys(savedData).length === 0) {
            return <Spinner/>
        }
        return (
            <div className="row">
                <div className="col-md-12">
                    <div className="card card-primary card-outline">
                        <h2 className="ml-3 mt-2">Welcome, {user.username}</h2>
                        <div className="card-body pt-3">
                            {this.renderForm()}
                        </div>

                        <div className="card-footer d-flex justify-content-end">
                            {editing ? (
                                <React.Fragment>
                                    <Button
                                        color="success"
                                        className="d-flex align-items-center mr-2"
                                        onClick={this.handleProfileUpdate}
                                    >
                                        <SaveOutlined/>
                                        Update Profile
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