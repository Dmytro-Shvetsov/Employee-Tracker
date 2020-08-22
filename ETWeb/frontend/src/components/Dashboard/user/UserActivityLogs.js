import React from 'react';
import axios from 'axios';
import * as authService from '../../../services/authService';
import {Spinner} from 'reactstrap';
import ImageGallery from 'react-image-gallery';
import "react-image-gallery/styles/css/image-gallery.css";
// import "react-image-gallery/styles/scss/image-gallery.scss";


export default class UserActivityLogs extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            socket: props.socket,
            data: {
                employee_id: props.match.params.id,
                since: (new Date(Date.now() - 86400000)).toISOString(),
            },
            allDataLoaded: false,
        };
        console.log((new Date(Date.now() - 86400000)).toISOString())
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

    loadProfileInfo = async () => {
        await this.cancelPreviousRequests();
        const {data} = this.state;
        try {
            const response = await authService.getUserProfile({user_id: data.employee_id}, this.reqSource.token);
            const profile = JSON.parse(response.data);
            console.log(profile);
            this.setState({profile});
        } catch (error) {
            console.log(error.message);
            if (error.response.data !== undefined) {
                console.log(error.response.data);
            }
        }
    };

    loadScreenshotLogs = async () => {
        await this.cancelPreviousRequests();
        const {data} = this.state;
        try {
            const response = await authService.getUserScreenshotActivityLogs(data, this.reqSource.token);
            const screenshots = JSON.parse(response.data);
            console.log(screenshots);
            this.setState({screenshots});
        } catch (error) {
            console.log(error.message);
            if (error.response.data !== undefined) {
                console.log(error.response.data);
            }
        }
    };

    loadDomainLogs = async () => {
        await this.cancelPreviousRequests();
        const {data} = this.state;
        try {
            const response = await authService.getUserDomainActivityLogs(data, this.reqSource.token);
            const hostnameCount = JSON.parse(response.data);
            console.log(hostnameCount);
            this.setState({hostnameCount});
        } catch (error) {
            console.log(error.message);
            if (error.response.data !== undefined) {
                console.log(error.response.data);
            }
        }
    };

    async componentDidMount() {
        this._isMounted = true;
        await this.loadProfileInfo();
        await this.loadScreenshotLogs();
        // await this.loadDomainLogs();
        this.setState({allDataLoaded: true});
    }

    async componentWillUnmount() {
        this._isMounted = false;
        await this.cancelPreviousRequests();
    }

    renderScreenshots() {
        const {screenshots = []} = this.state;

        return (
            <React.Fragment>
                {screenshots.length === 0 ? (
                    <span className="text-muted">
                        There are no screenshots for past 24 hours.
                    </span>
                ) : (
                    <ImageGallery showPlayButton={false}
                                  onClick={() => {document.getElementsByClassName("image-gallery-fullscreen-button")[0].click()}}
                                  items={screenshots.map(item => ({
                            original: item.image,
                            thumbnail: item.image,
                        }))
                    }/>
                )}
            </React.Fragment>
        );

    }

    renderProfile() {
        const {profile, status="offline"} = this.state;
        return (
            <React.Fragment>
                <div>
                    <h1 className="mb-0 text-truncated">
                        {profile.full_name}
                    </h1>
                </div>
                <span className="font-italic">
                    (currently <span className={status}>{status}</span>)
                </span>
                <p className="lead">{profile.work_place}</p>
            </React.Fragment>
        );
    }

    render() {
        const {allDataLoaded} = this.state;
        if (!allDataLoaded) {
            return <Spinner/>
        }

        const {profile} = this.state;
        return (
            <div className="card">
                <div className="card-body">
                    <div className="row">
                        <div className="col-12 col-lg-8 col-md-6">
                            {this.renderProfile()}
                            <div className="card-text">
                                <h6>
                                    Contents
                                </h6>
                                <ul>
                                    <li><a href="#screenshots-heading">Screenshots</a></li>
                                    <li><a href="#domains-heading">Domains</a></li>
                                </ul>
                                {/*<div>*/}
                                {/*    <a href="#">Past day</a>*/}
                                {/*    <a href="#">Past week</a>*/}
                                {/*    <a href="#">Past month</a>*/}
                                {/*</div>*/}

                            </div>
                        </div>
                        <div className="col-12 col-lg-4 col-md-6 text-center">
                            <img src={profile.image} alt="Profile image" id="profile-image-rounded"/>
                        </div>
                    </div>
                    <div className="row" style={{margin: "1rem 5rem"}}>
                        <h5 className="d-block text-center" style={{width: "100%"}}>
                            <a id="screenshots-heading">Recent screenshots</a>
                        </h5>
                        <div className="col-12" style={{width: "100%"}}>
                            {this.renderScreenshots()}
                        </div>
                        <h5 className="d-block text-center" style={{width: "100%"}}>
                            <a id="domains-heading">Recent visited websites</a>
                        </h5>
                        <div className="col-12" style={{width: "100%"}}>
                            12333123
                        </div>
                    </div>
                </div>
            </div>
        );
    }

}
