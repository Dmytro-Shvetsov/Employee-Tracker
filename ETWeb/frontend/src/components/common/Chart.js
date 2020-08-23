import React from 'react';
import PropTypes from 'prop-types';
import Chart from 'chart.js/dist/Chart.min';
import 'chart.js/dist/Chart.min.css';


export default class ChartComponent extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            config: props.config,
            canvasId: props.id,
            className: props.className,
        };
        this.canvasRef = React.createRef();
    }

    componentDidMount() {
        const ctx = this.canvasRef.current.getContext("2d");
        this.setState({
            chartObject: new Chart(ctx, this.state.config)
        });
    }

    render() {
        const { canvasId, className } = this.state;
        return <canvas id={canvasId} className={className} ref={this.canvasRef}/>;
    }
}

ChartComponent.propTypes = {
    config: PropTypes.shape({
        // data: PropTypes.object.isRequired
    }).isRequired,
    className: PropTypes.string,
    id: PropTypes.string
};

ChartComponent.defaultProps = {
    id: "",
    className: ""
};